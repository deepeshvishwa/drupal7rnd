<?php

/**
 * @file
 * Administrative functionality for Logged-in users list.
 */

/**
 * Display list of logged-in users who have logged-in verious machines/browsers.
 */
function loggedin_users_list_page() {
  // Display number of users currently who have loggedin.
  $users_no = db_select('sessions')
  ->fields('sessions', array('uid'))
  ->distinct()
  ->countQuery()->execute()->fetchField();

  $query = db_select('sessions', 's');
  $query->join('users', 'u', 's.uid = u.uid');
  $query->addExpression('COUNT(s.uid)', 'count_ip');
  $query->fields('s',array('uid', 'hostname', 'timestamp', 'sid'));
  $query->fields('u',array('name'));
  $query->groupBy('s.uid');
  $query->orderBy('uid', 'ASC');
  $users_list = $query->execute();

  $header = array(
    t('User id'),
    t('Username'),
    t('Number of Logged-in (Devices or Browsers)'),
    t('HostName'),
    t('Loggedin Time'),
    t('Operations'),
  );

  $rows = array();

  foreach ($users_list as $user_list) {
    $user_name = l(t($user_list->name), "/user/$user_list->uid");

    if ($user_list->count_ip > 1) {

      $header_inner = array('','');
      $rows_inner = array();

      $query = db_select('sessions', 's');
      $query->fields('s',array('hostname', 'timestamp', 'sid'));
      $query->condition('uid', $user_list->uid, '=');
      $query->orderBy('timestamp', 'DESC');
      $detailed_list = $query->execute();

      foreach ($detailed_list as $ls) {
        $loggedin_time_inner = format_date($ls->timestamp, 'custom', 'Y-m-d H:i:s');
        $logout_user_inner = l(t('Logout'), "/admin/config/people/loggedin-users-list/logout/$ls->sid");

        $rows_inner[] = array(
          $ls->hostname,
          $loggedin_time_inner,
          $logout_user_inner,
        );
      }

      $table_data_inner = theme('table', array('rows' => $rows_inner));
      $value_host = array('data' => $table_data_inner, 'colspan' => 3);
      $rows[] = array(
        $user_list->uid,
        $user_name,
        $user_list->count_ip,
        $value_host,
      );
    }
    else {
      $logout_user = l(t('Logout'), "/admin/config/people/loggedin-users-list/logout/$user_list->sid");;
      $loggedin_time = format_date($user_list->timestamp, 'custom', 'Y-m-d H:i:s');
      $rows[] = array(
        $user_list->uid,
        $user_name,
        $user_list->count_ip,
        $user_list->hostname,
        $loggedin_time,
        $logout_user,
      );
    }
  }

  $table_data = theme('table', array('header' => $header, 'rows' => $rows));

  return theme("loggedin_users_list", array('users_no' => $users_no, 'userdata' => $table_data));
}

/**
 * Selected user log out from here.
 */
function logout_user_custom_page($sid) {
  $status = db_delete('sessions')
    ->condition('sid', $sid)
    ->execute();

  drupal_set_message(t('User log out successfully.'));
  drupal_goto('/admin/config/people/loggedin-users-list');
}
