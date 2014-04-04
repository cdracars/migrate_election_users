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
    $this->highwaterField = array();
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
    //$this->addFieldMapping('roles')
    //     ->defaultValue(DRUPAL_AUTHENTICATED_RID);

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
      //'role_names',
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
    
    $arr = explode('   ', $row->major);
    $row->major = $arr[0];
  }
  
  public function prepare($node, stdClass $row) {
    //watchdog('Classification Import', '<pre>' . print_r($row, TRUE) . '</pre>');
    $role = new stdClass();
    $role->name = $row->class;
    $exists = user_roles();
    $div_art = array('Art', 'Music', 'Communication', 'Drama', 'English');
    $div_science = array('Physical Education', 'Mathematics', 'Biology', 'Chemistry', 'Natural Science', 'Physics');
    $div_buisness = array('Business', 'American Indian Studies', 'Economics', 'History', 'Political Science', 'Psychology', 'Sociology');
    $div_education = array('Early Childhood', 'Education of the Deaf', 'Elementary Education', 'Speech Language Pathology');
    //watchdog('Classification Import', '<pre>' . print_r($exists, TRUE) . '</pre>');
    if (!in_array($row->class, $exists)) {
      user_role_save($role);
    }
    // Assign role to user here
    foreach($exists as $rid => $role_name) {
      if ($role_name == $role->name) {
        $node->roles[$rid] = $role_name;
      }
    }
    if (in_array($row->major, $div_art)) {
      $role->name = 'Divison of Arts and Humanities';

      if (!in_array($role->name, $exists)) {
        user_role_save($role);
      }
      // Assign role to user here
      foreach($exists as $rid => $role_name) {
        if ($role_name == $role->name) {
          $node->roles[$rid] = $role_name;
        }
      }
    } 
    elseif (in_array($row->major, $div_science)) {
      $role->name = 'Divison of Science and Physical Education';

      if (!in_array($role->name, $exists)) {
        user_role_save($role);
      }
      // Assign role to user here
      foreach($exists as $rid => $role_name) {
        if ($role_name == $role->name) {
          $node->roles[$rid] = $role_name;
        }
      }
    } 
    elseif (in_array($row->major, $div_buisness)) {
      $role->name = 'Divison of Buisness and Social Science';

      if (!in_array($role->name, $exists)) {
        user_role_save($role);
      }
      // Assign role to user here
      foreach($exists as $rid => $role_name) {
        if ($role_name == $role->name) {
          $node->roles[$rid] = $role_name;
        }
      }
    } 
    elseif (in_array($row->major, $div_education)) {
      $role->name = 'Divison of Education and Speech Language Pathology';

      if (!in_array($role->name, $exists)) {
        user_role_save($role);
      }
      // Assign role to user here
      foreach($exists as $rid => $role_name) {
        if ($role_name == $role->name) {
          $node->roles[$rid] = $role_name;
        }
      }
    } 
    else {
      //watchdog('Majors Role Import Fail', '<pre>' . print_r($row->major, TRUE) . '</pre>');
      watchdog('Majors Role Import Fail', '<pre>' . print_r($node, TRUE) . '</pre>');
    }
  }
}

