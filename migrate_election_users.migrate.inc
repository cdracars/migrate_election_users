<?php

/**
 * @file
 *   A baseball game migration example.
 */


/*
 * You must implement hook_migrate_api(), setting the API level to 2, for
 * your migration classes to be recognized by the Migrate module.
 */
function migrate_election_users_migrate_api() {
  $api = array(
    'api' => 2,
    'groups' => array(
      'SGAMigrate' => array(
        'title' => t('SGAMigrate Imports'),
      ),
    ),
    'migrations' => array(
      'SGA_Election_User' => array(
        'class_name' => 'SgaElectionUserMigration',
        'group_name' => 'SGAMigrate',
      ),
    ),
  );
  return $api;
}

/**
 * A migration that is reused for each source CSV file.
 */
class SgaElectionUserMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('SGA Election User Migration');
    $columns = array(
      // "Source": ('Fieldname', 'Description')
      2 => array('sid', t('Student ID (must be unique)')),
      3 => array('email', t('Email (Account)')),
      4 => array('class', t('User Classification')),
      5 => array('major', t('User Major'))
    );
    // TIP: delete ", array('header_rows' => 1)" in the next line, if the CSV-file has NO header-line
    $this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_election_users') . '/data_sources/drupaluser_import.csv', $columns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));
    $this->map = new MigrateSQLMap($this->machineName,
        array('sid' => array( 
                'type' => 'varchar',
                'length' => 10,
                'not null' => TRUE,
                'description' => t('Student ID') // description never used
              )
             ),
        MigrateDestinationUser::getKeySchema()
    );

    $this->addFieldMapping('name', 'email');

    $this->addFieldMapping('mail', 'email');

    $this->addFieldMapping('pass', 'sid');

    $this->addFieldMapping('field_classification', 'class');

    $this->addFieldMapping('field_major', 'major');

    // Instead of mapping a source field to a destination field, you can
    // hardcode a default value. You can also use both together - if a default
    // value is provided in addition to a source field, the default value will
    // be applied to any rows where the source field is empty or NULL.
    $this->addFieldMapping('roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);

    // Unmapped destination fields

    // This is a shortcut you can use to mark several destination fields as DNM
    // at once.
    $this->addUnmigratedDestinations(array(
      'access',
      'created',
      'data',
      'is_new',
      'language',
      'login',
      'picture',
      'role_names',
      'signature',
      'signature_format',
      'theme',
      'timezone',
    ));

    $this->addFieldMapping('int')
         ->issueGroup(t('DNM'));

    $destination_fields = $this->destination->fields();
    if (isset($destination_fields['path'])) {
      $this->addFieldMapping('path')
           ->issueGroup(t('DNM'));
      if (isset($destination_fields['pathauto'])) {
        $this->addFieldMapping('pathauto')
             ->issueGroup(t('DNM'));
      }
    }
  }

  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $major = $row->major;
    $arr = explode('   ', trim($major));
    $row->major = $arr;
  }
}

