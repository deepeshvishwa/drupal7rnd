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
  $users_no = db_query("SELECT COUNT(DISTINCT(uid)) FROM {sessions}")->fetchfield();

  // Display List of currently loggedin users.
  $users_list = db_query("SELECT count(s.uid) count_ip, s.uid, u.name, s.hostname, FROM_UNIXTIME(s.timestamp) loggedin_time, sid FROM {sessions} s LEFT JOIN {users} u ON u.uid = s.uid GROUP BY s.uid ORDER BY s.uid")->fetchAll();

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

    $dt = new DateTime($user_list->loggedin_time);
    $loggedin_time = $dt->format('Y-m-d H:i:s');

    $detailed_list = db_query("SELECT hostname, FROM_UNIXTIME(timestamp) loggedin_time, sid FROM {sessions} WHERE uid= :uid ORDER BY loggedin_time DESC", array(':uid' => $user_list->uid))->fetchAll();

    if ($user_list->count_ip > 1) {

      $header_inner = array('', '');
      $rows_inner = array();

      foreach ($detailed_list as $ls) {
        $dt1 = new DateTime($ls->loggedin_time);
        $loggedin_time_inner = $dt1->format('Y-m-d H:i:s');
        $logout_user1 = '<a href="/admin/config/people/loggedin-users-list/logout/' . $ls->sid . '">Logout</a>';
        $rows_inner[] = array(
          $ls->hostname,
          $loggedin_time_inner,
          $logout_user1,
        );
      }
      $table_data_inner = theme('table', array('rows' => $rows_inner));

      $value_host = array('data' => $table_data_inner, 'colspan' => 3);
      $rows[] = array(
        $user_list->uid,
        $user_list->name,
        $user_list->count_ip,
        $value_host,
      );
    }
    else {
      $logout_user = '<a href="/admin/config/people/loggedin-users-list/logout/' . $user_list->sid . '">Logout</a>';
      $rows[] = array(
        $user_list->uid,
        $user_list->name,
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
