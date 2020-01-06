Automatic Entity Label
======================

Automatic Entity Label allows hiding of entity label fields. To prevent empty
labels it can be configured to generate the label automatically by a given
pattern. The module can be used for any entity type that has a label, including
node titles, comment subjects, taxonomy term names and profile2 labels.

Patterns for automatic labels are constructed with the help of tokens. Backdrop
core provides a basic set of tokens and a token selection widget. Some entity
types (e.g. profile) provide tokens via the entity_token module.

Advanced users can use PHP code for automatically generating labels. See below
for more information.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  <https://backdropcms.org/guide/modules>
- Configure automatic labels for each entity type at that entity type's bundle
  configuration page (e.g. `admin/structure/types` for content types).

Notes
-----

1. Automatic entity label also works with title replacements provided by the
   Title module.
2. When you change the pattern for automatic labels, existing entities are not
   updated automatically.

Advanced Use: PHP Code
----------------------

Users who have the 'use PHP for label patterns' permission can use PHP code
within patterns for automatic label genereration.

Here is a simple example, which just adds the node's author as title:

  `<?php return "Author: $entity->name"; ?>`

Two variables are available for use:

- `$entity`: the entity for which the label is generated
- `$language`: the intended language of the label

You can also combine tokens with PHP evaluation. Be aware that this can lead to
security holes if you use textual values provided by users. If in doubt, avoid
combining tokens with PHP evaluation.

Here is an example, which sets an entity label to the value of a field
(field_testnumber), or the entity bundle (node type) if the field value is
empty.

```php
$token = '[field_testnumber]';
if (empty($token)) {
  return '[type]';
}
else {
  return $token;
}
```

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Credits
-------

This module was originally written for Drupal by Wolfgang Ziegler
<https://github.com/fago>, based on the Automatic Nodetitles module
<https://www.drupal.org/project/auto_nodetitle>.
