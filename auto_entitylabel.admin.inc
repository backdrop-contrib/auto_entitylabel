<?php
/**
 * @file
 * Contains administration forms.
 */

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
  );
  $form['auto_entitylabel']['auto_entitylabel_status'] = array(
    '#type' => 'radios',
    '#default_value' => $config['status'],
    '#options' => _auto_entitylabel_options($entity_type, $bundle_name),
  );
  $form['auto_entitylabel']['auto_entitylabel_pattern'] = array(
    '#type' => 'textarea',
    '#title' => t('Pattern for the title'),
    '#description' => t('Leave blank for using the per default generated title. Otherwise this string will be used as title. Use the syntax [token] if you want to insert a replacement pattern.'),
    '#default_value' => $config['pattern'],
  );
  // Don't allow editing of the pattern if PHP is used, but the users lacks
  // permission for PHP.
  if ($config['php'] && !user_access('use PHP for label patterns')) {
    $form['auto_entitylabel']['auto_entitylabel_pattern']['#disabled'] = TRUE;
    $form['auto_entitylabel']['auto_entitylabel_pattern']['#description'] = t('You are not allow the configure the pattern for the title, as you lack the %permission permission.', array('%permission' => t('Use PHP for title patterns')));
  }
  $form['auto_entitylabel']['auto_entitylabel_php'] = array(
    '#access' => user_access('use PHP for label patterns'),
    '#type' => 'checkbox',
    '#title' => t('Evaluate PHP in pattern.'),
    '#description' => t('Put PHP code above that returns your string, but make sure you surround code in <code>&lt;?php</code> and <code>?&gt;</code>. Note that <code>$entity</code> and <code>$language</code> are available and can be used by your code.'),
    '#default_value' => $config['php'],
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
 * @param $entity_type
 *   The entity type.
 * @param $bundle_name
 *   The name of the bundle.
 *
 * @return
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

