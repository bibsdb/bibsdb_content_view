<?php
/**
 * @file
 * Custom field node views handler.
 * Calculate usage of nodes in search context links, search contexts, exposures and menus.
 *
 */

/**
 * Custom handler class.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_node_bibsdb_node_usage extends views_handler_field_node {

      /**
   * {@inheritdoc}
   *
   * Perform any database or cache data retrieval here. In this example there is
   * none.
   */
      function query() {

      }

   /**
   * {@inheritdoc}
   *
   * Modify any end user views settings here. Debug $options to view the field
   * settings you can change.
   */
   function option_definition() {
    $options = parent::option_definition();
    return $options;
  }

  /**
   * {@inheritdoc}
   *
   * Make changes to the field settings form seen by the end user when adding
   * your field.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }


  /**
   * Render callback handler.
   *
   * Return the markup that will appear in the rendered field.
   */
  function render($values) {

    $nid = $values->nid;
    $nids = [];

    // Create an array with the original nid and nids of all search context links pointing to this node node
    $linknids = $this->_detect_search_context_links($nid);
    $nids = $linknids;
    $nids[] = $nid;

    $menu_item = $this->_detect_menu_item($nids);
    $exposures = $this->_detect_exposures($nids);
    $contexts = $this->_detect_contexts($nids);

    $content = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('bibsdb-node-usage'),
        ),
      );

    $content['usage-button'] = array(
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#value' => 0,
      '#attributes' => array(
        'class' => array('usage-button'),
        'type' => array('button'),
        ),
      );

    if(!(empty($linknids))) {
      $content['link-nids'] = array(
        '#type' => 'html_tag',
        '#tag' => 'span',
        '#value' => 'Link(s): ' . implode(" ", $linknids),
        '#attributes' => array(
          'class' => array('link-nids'),
          ),
        );
    }

    $content['usage-details'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('usage-details'),
        ),
      );

    if(!(empty($menu_item))) {
      $content['usage-button']['#value']++;
      $content['usage-details']['menu-item'] = $menu_item;
    }

    $i = 0;
    foreach ($exposures as $exposure) {
      $content['usage-button']['#value']++;
      $content['usage-details']['exposure'][$i++] = $exposure;
    }

    $i = 0;
    foreach ($contexts as $context) {
      $content['usage-button']['#value']++;
      $content['usage-details']['context'][$i++] = $context;
    }

    return $content;
  }


 /**
  * If the node og any of it links has a menu entry return the link title.
  *
  */

  function _detect_menu_item($nids) {
    $paths = [];

    foreach ($nids as $nid) {
      $paths[] = 'node/'. $nid;
    }

    $mlids = db_select('menu_links' , 'ml')
    ->condition('ml.link_path', ($paths), 'IN')
    ->condition('ml.menu_name', 'main-menu')
    ->fields('ml' , array())
    ->range(0, 1)
    ->execute()
    ->fetchAll();

    $content = [];
    if(!(empty($mlids))) {
      $linktitle = $mlids[0]->link_title;
      $menulink = menu_link_load($mlids[0]->mlid);

      $parentlink = menu_link_load($menulink['plid']);

      $parenttitle = $parentlink['link_title'];

      $content['menu-item-markup'] = array(
        '#prefix' => '<div>',
        '#type' => 'markup',
        '#markup' => 'Menu item: ' . $parenttitle . ' -> ' . $linktitle,
        '#suffix' => '</div>',
        );
    }
    return $content;
  }


 /**
  * If the node or any of its links are attached to exposures return the hosting nodes (if any).
  *
  */

  function _detect_exposures($nids) {
    $query = new EntityFieldQuery();
    $result = $query->entityCondition('entity_type', 'exposure', '=')
    ->fieldCondition('field_bibsdb_brick_content', 'target_id', $nids, 'IN')
    ->execute();

    $exposures = [];
    if (isset($result['exposure'])) {
      $exposure_ids = array_keys($result['exposure']);
      $exposures = entity_load('exposure', $exposure_ids);
    } 


    $content = [];
    foreach ($exposures as $exposure) {


      $wrapper = entity_metadata_wrapper('exposure', $exposure);
      
      $hosting_node = $wrapper->field_bibsdb_hosting_node->value();
      $exposure_markup = array(
        '#prefix' => '<div>',
        '#type' => 'markup',
        '#markup' => 'Exposed on:' . $hosting_node->title . '(' . $hosting_node->nid . ')',
        '#suffix' => '</div>',
        );

      array_push($content, $exposure_markup);

    }

    return $content;
  }


 /**
  * If the node or any of its links are attached to search contexts return the name of the contexts.
  *
  */   

  function _detect_contexts($nids) {

    $content = [];
    $query = db_select('ting_search_context_nodes_rated', 'nr');
    $query->join('ting_search_context_contexts', 'co', 'co.context_id = nr.context_id');
    $query->fields('co', array('name'));
    $query->condition('nid', $nids, 'IN');
    $result = $query->execute()->fetchAll();

    foreach ($result as $obj) {
      $context_markup = array(
        '#prefix' => '<div>',
        '#type' => 'markup',
        '#markup' => 'In context: ' . $obj->name,
        '#suffix' => '</div>',
        );

      $content[] = $context_markup;

    }
    

    return $content;
  }

 /**
  * If the node is attached to any search context links return the nids of the links.
  *
  */

  function _detect_search_context_links($nid) {

    $nids = [];
    $query = new EntityFieldQuery();
    $result = $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'ding_search_context_link')
    ->fieldCondition('field_bibsdb_internal_link', 'target_id', $nid, '=')
    ->execute();


    if (isset($result['node'])) {
      $nids = array_keys($result['node']);
    } 

    return $nids;
  }
}

