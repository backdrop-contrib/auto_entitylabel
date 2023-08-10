<?php
/**
 * @file
 * Contains administration forms.
 */

/**
 * Administration page.
 */
function auto_entitylabel_settings_default_page() {

  $content_types = node_type_get_types();

  $markup = '<div>';
  $markup .= '<fieldset>';
  $markup .= '<legend>';
  $markup .= '<span class = "fieldset-legend">';
  $markup .= '<a class = "fieldset-title" href = "#">' . t('Auto Label') . '</a></span>';
  $markup .= '</legend>';
  $markup .= '<div class = "fieldset-wrapper">';
  $markup .= '<div>' . t('Configure Auto Label for Content Type.') . '</div><br>';

  $markup .= '<ul class="admin-list">';

  foreach ($content_types as $content_types_machine_name => $content_type) {
    $content_type_name = $content_type->name;
    $content_types_machine_name = $content_type->type;
    $markup .= '<li class="leaf">';
    $markup .= '<a href="' . base_path() . 'admin/structure/types/manage/' . $content_types_machine_name . '/auto_label">' . $content_type_name . '</a>';
    $markup .= '<div class="description">' . t('AUTOMATIC LABEL GENERATION FOR @content_type_name', array('@content_type_name' => strtoupper($content_type_name))) . '</div>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';
  $markup .= '</fieldset>';
  $markup .= '</div>';

  $markup .= '<div>';
  $markup .= '<fieldset>';
  $markup .= '<legend>';
  $markup .= '<span class = "fieldset-legend">';
  $markup .= '<a class = "fieldset-title" href = "#">' . t('Comment Auto Label') . '</a></span>';
  $markup .= '</legend>';
  $markup .= '<div class = "fieldset-wrapper">';
  $markup .= '<div>' . t('Configure Auto Label for Comments in Content Type.') . '</div><br>';

  $markup .= '<ul class="admin-list">';

  foreach ($content_types as $content_types_machine_name => $content_type) {
    $content_type_name = $content_type->name;
    $content_types_machine_name = $content_type->type;
    $markup .= '<li class="leaf">';
    $markup .= '<a href="' . base_path() . 'admin/structure/types/manage/' . $content_types_machine_name . '/comment/auto_label">' . $content_type_name . '</a>';
    $markup .= '<div class="description">' . t('AUTOMATIC LABEL GENERATION FOR @content_type_name COMMENTS', array('@content_type_name' => strtoupper($content_type_name))) . '</div>';
    $markup .= '</li>';
  }

  $markup .= '</ul>';
  $markup .= '</div>';
  $markup .= '</fieldset>';
  $markup .= '</div>';

  return array(
    '#markup' => $markup,
  );
}

/**
 * Administration form for each bundle.
 */
function auto_entitylabel_settings_form($form, $form_state, $entity_type, $bundle_arg) {
  $info = entity_get_info($entity_type);
  $bundle_name = is_object($bundle_arg) ? $bundle_arg->{$info['bundle keys']['bundle']} : $bundle_arg;
  $bundle_label = $info['bundles'][$bundle_name]['label'];
  $key = $entity_type . '_' . $bundle_name;
  $config = config_get('auto_entitylabel.settings', $key);

  $form['auto_entitylabel'] = array(
    '#type' => 'fieldset',
    '#title' => t('Automatic label generation for @type', array('@type' => $bundle_label)),
    '#weight' => 0,
    '#attached' => array(
      'js' => array(
        'auto-entitylabel' =>
         backdrop_get_path('module', 'auto_entitylabel') . '/js/auto_entitylabel.admin.js',
      ),
    ),
  );
  $form['auto_entitylabel']['auto_entitylabel_status'] = array(
    '#type' => 'radios',
    '#default_value' => isset($config['status']) ? $config['status'] : AUTO_ENTITYLABEL_DISABLED,
    '#options' => _auto_entitylabel_options($entity_type, $bundle_name),
  );
  $form['auto_entitylabel']['auto_entitylabel_pattern'] = array(
    '#type' => 'textarea',
    '#title' => t('Pattern for the title'),
    '#description' => t('Leave blank for using the per default generated title. Otherwise this string will be used as title. Use the syntax [token] if you want to insert a replacement pattern.'),
    '#default_value' => isset($config['pattern']) ? $config['pattern'] : NULL,
    '#attributes' => array('class' => array('pattern-label')),
  );
  // Don't allow editing of the pattern if PHP is used, but the users lacks
  // permission for PHP.
  if (isset($config['php']) && $config['php'] && !user_access('use PHP for label patterns')) {
    $form['auto_entitylabel']['auto_entitylabel_pattern']['#disabled'] = TRUE;
    $form['auto_entitylabel']['auto_entitylabel_pattern']['#description'] = t('You are not allow the configure the pattern for the title, as you lack the %permission permission.', array('%permission' => t('Use PHP for title patterns')));
  }
  $form['auto_entitylabel']['auto_entitylabel_php'] = array(
    '#access' => user_access('use PHP for label patterns'),
    '#type' => 'checkbox',
    '#title' => t('Evaluate PHP in pattern.'),
    '#description' => t('Put PHP code above that returns your string, but make sure you surround code in <code>&lt;?php</code> and <code>?&gt;</code>. Note that <code>$entity</code> and <code>$language</code> are available and can be used by your code.'),
    '#default_value' => isset($config['php']) ? $config['php'] : FALSE,
  );
  $form['auto_entitylabel']['auto_entitylabel_strip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Strip HTML tags from generated title.'),
    '#description' => t('Disable to prevent striping HTML tags from entity labels.'),
    '#default_value' => isset($config['strip']) ? $config['strip'] : FALSE,
  );

  // Display the list of available placeholders.
  $form['auto_entitylabel']['token_help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array($info['token type']),
    '#dialog' => TRUE,
  );

  // Add $key to form_state.
  $form['auto_entitylabel']['auto_entitylabel_key'] = array(
    '#type' => 'value',
    '#value' => $key,
  );

  // Add a submit button.
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for auto_entitylabel_settings_form().
 */
function auto_entitylabel_settings_form_submit($form, &$form_state) {
  config_set('auto_entitylabel.settings', $form_state['values']['auto_entitylabel_key'], array(
    'status' => $form_state['values']['auto_entitylabel_status'],
    'pattern' => $form_state['values']['auto_entitylabel_pattern'],
    'php' => $form_state['values']['auto_entitylabel_php'],
  ));

  backdrop_set_message(t('The configuration options have been saved.'));
}

/**
 * Constructs the list of options for the given bundle.
 */
function _auto_entitylabel_options($entity_type, $bundle_name) {
  $options = array(
    AUTO_ENTITYLABEL_DISABLED => t('Disabled'),
  );

  if (auto_entitylabel_entity_label_visible($entity_type, $bundle_name)) {
    $options += array(
      AUTO_ENTITYLABEL_ENABLED => t('Automatically generate the label and hide the label field'),
      AUTO_ENTITYLABEL_OPTIONAL => t('Automatically generate the label if the label field is left empty'),
    );
  }
  else {
    $options += array(
      AUTO_ENTITYLABEL_ENABLED => t('Automatically generate the label'),
    );
  }

  return $options;
}

/**
 * Check if given entity bundle has a visible label on the entity form.
 *
 * @param string $entity_type
 *   The entity type.
 * @param string $bundle_name
 *   The name of the bundle.
 *
 * @return hidden[entity_type]
 *   TRUE if the label is rendered in the entity form, FALSE otherwise.
 *
 * @todo
 *   Find a generic way of determining the result of this function. This
 *   will probably require access to more information about entity forms
 *   (entity api module?).
 */
function auto_entitylabel_entity_label_visible($entity_type, $bundle_name) {
  $hidden = array(
    'profile2' => TRUE,
  );

  return empty($hidden[$entity_type]);
}

