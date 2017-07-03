
<?php
/**
 * Implements hook_views_data().
 */
function bibsdb_content_view_views_data() {
  $data['node']['bibsdb_node_usage'] = array(
    'field' => array(
      'title' => t('Node Usage'),
      'help' => t('Provide info about a nodes usage in menus, exposures and search context links.'),
      'handler' => 'views_handler_field_node_bibsdb_node_usage',
      ),
    );
  return $data;
}