<?php

function fpeditor_add_content_wizard_next(&$form_state) {
  fpeditor_add_content_cache_set($form_state['object_id'], $form_state['object']);
}

function fpeditor_add_content_wizard_finish(&$form_state) {
  $form_state['complete'] = TRUE;
}

function fpeditor_add_content_wizard_cancel(&$form_state) {
  $form_state['cancel'] = TRUE;
}

function fpeditor_add_content_form_wizard_start($form,&$form_state){
	drupal_set_title(t('Select content type to add'));
	$form['idWidget'] = array(
		'#type' => 'hidden',
		'#default_value' => $form_state['object']->idWidget,
	);
	
	$form['content_type'] = array(
    '#type' => 'radios',
    '#title' => t('Content type'),
    '#default_value' =>  isset($form_state['object']->type)?$form_state['object']->type:'node',
    '#options' => array('node'=>'Node','block'=>'Block','text-area'=>'Text Area','links'=>t('Links'),'media'=>'Media'),
    '#description' => t('Select content type to add.'),
	);
	return $form;
}

function fpeditor_add_content_form_wizard_start_submit($form,&$form_state){
	$form_state['object']->type = $form_state['values']['content_type'];
	$form_state['object']->idWidget = $form_state['values']['idWidget'];
}

function fpeditor_add_content_form_wizard_second($form,&$form_state){
	$form['idWidget'] = array(
		'#type' => 'hidden',
		'#default_value' => $form_state['object']->idWidget,
	);
	if($form_state['object']->type=='node'){
		drupal_set_title('Add node');
		$entity_info = entity_get_info('node');
		$view_modes = array();
		foreach($entity_info['view modes'] as $key=>$view_mode){
			$view_modes[$key] = $view_mode['label'];
		}
		$form['search_node'] = array(
			'#type' => 'textfield',
			'#title' => t('Search node'),
			'#autocomplete_path' => 'fpeditor/autocomplete_node',
			'#default_value' => isset($form_state['object']->node)?$form_state['object']->node:NULL,
		);
		$form['view_mode'] = array(
			'#type' => 'select',
			'#title' => t('View mode'),
			'#options' => $view_modes,
			'#default_value' => isset($form_state['object']->view_mode)?$form_state['object']->view_mode:'teaser',
		);
	} elseif ($form_state['object']->type=='block'){
		drupal_set_title('Add block');
		require_once(drupal_get_path('module','block').'/block.admin.inc');
    $theme = variable_get('theme_default', 'garland');
    $blocks = array();
    foreach(block_admin_display_prepare_blocks($theme) as $block){
			$blocks[$block['module'].'|'.$block['delta']] = $block['info'];
		}
		$form['select_block'] = array(
			'#type' => 'select',
			'#title' => t('Select block'),
			'#options' => $blocks,
			'#default_value' => isset($form_state['object']->block)?$form_state['object']->block:NULL,
		);
		$form['block_setting'] = array(
			'#type' => 'radios',
			'#title' => t('Display block mode'),
			'#description' => t('Static: display actual content. Dynamic: display content generate on the fly.'),
			'#options' => array('static'=>'Static','dynamic'=>'Dynamic'),
			'#default_value' => isset($form_state['object']->block_setting)?$form_state['object']->block_setting:'dynamic',
		);
	} elseif ($form_state['object']->type=='text-area'){
		drupal_set_title('Add text area');
		$form['text_area'] = array(
			'#type' => 'text_format',
			'#title' => t('Text area'),
			'#format' => 'full_html'
		);
	}
	return $form;
}
				 
function fpeditor_add_content_form_wizard_second_submit($form,&$form_state){
	if(isset($form_state['values']['search_node'])){ 
		$form_state['object']->node = $form_state['values']['search_node'];
		$form_state['object']->view_mode = $form_state['values']['view_mode'];
	}
	if(isset($form_state['values']['select_block'])){ 
		$form_state['object']->block = $form_state['values']['select_block'];
		$form_state['object']->block_setting = $form_state['values']['block_setting'];
	}
	$form_state['object']->idWidget = $form_state['values']['idWidget'];
}

function fpeditor_add_content_form_wizard_third($form,&$form_state){
	$form['idWidget'] = array(
		'#type' => 'hidden',
		'#default_value' => $form_state['object']->idWidget,
	);
	$content ='';
	if ($form_state['object']->type == 'node'){
		$nid = substr( $form_state['object']->node,1, strpos($form_state['object']->node,')')-1 );
		$node = node_load($nid);
		$content = render(node_view($node, $form_state['object']->view_mode, NULL));
	} elseif($form_state['object']->type == 'block'){
		$block = explode('|',$form_state['object']->block);
		$content = module_invoke($block[0], 'block_view', $block[1])['content'];
	}
	$form['content'] = array(
    '#type' => 'text_format',
    '#title' => t('Content'),
    '#default_value' => $content,
    '#description' => t('Now, you can edit this before insert'),
    '#format' => 'full_html',
	);
	return $form;	
}

function fpeditor_add_content_form_wizard_third_submit($form,&$form_state){
	$form_state['object']->content = $form_state['values']['content']['value'];
	$form_state['object']->idWidget = $form_state['values']['idWidget'];
}


function fpeditor_add_content_get_result($object){
	return $object->content;
}

function fpeditor_add_content_cache_clear($id) {
  ctools_include('object-cache');
  ctools_object_cache_clear('fpeditor_add_content', $id);
}

function fpeditor_add_content_cache_set($id, $object) {
  ctools_include('object-cache');
  ctools_object_cache_set('fpeditor_add_content', $id, $object);
}

function fpeditor_add_content_cache_get($id) {
  ctools_include('object-cache');
  $object = ctools_object_cache_get('fpeditor_add_content', $id);
  if (!$object) {
    $object = new stdClass;
  }
  return $object;
}


function fpeditor_add_content_modal_form($js = NULL, $step = NULL){
	if ($js) {
    ctools_include('modal');
    ctools_include('ajax');
  }
	
	$form_info = array(
		'id' => 'fpeditor_add_content_modal_form',
		'path' => 'fpeditor/'. ($js ? 'ajax' : 'nojs') .'/add_content_modal_form/%step',
		'show trail' => FALSE,
    'show back' => TRUE,
    'show cancel' => TRUE,
    'show return' => FALSE,
    'next callback' =>  'fpeditor_add_content_wizard_next',
    'finish callback' => 'fpeditor_add_content_wizard_finish',
    'cancel callback' => 'fpeditor_add_content_wizard_cancel',
    'order' => array(
			'start' => t('Select content type'),
			'second' => t('Select node'),
			'third' => t('Confirm'),
    ),
    'forms' => array(
			'start' => array(
				'form id' => 'fpeditor_add_content_form_wizard_start'
			),
			'second' => array(
				'form id' => 'fpeditor_add_content_form_wizard_second'
			),
			'third' => array(
				'form id' => 'fpeditor_add_content_form_wizard_third'
			),
    ),
	);
	
	$firstExec = FALSE;
	$object_id = 1;
	if (empty($step)){
		fpeditor_add_content_cache_clear($object_id);
		$step = 'start';
		$firstExec = TRUE;
	}
	
	$object = fpeditor_add_content_cache_get($object_id);
	
	$form_state = array(
    'ajax' => $js,
    'object_id' => $object_id,
    'object' => &$object,
  );
  
  if($firstExec){
		$form_state['object']->idWidget = '#grid-stack-item-'.$_GET['widget'].' .grid-stack-item-content';
	}
  
  ctools_include('wizard');
  $form = ctools_wizard_multistep_form($form_info, $step, $form_state);
  $output = drupal_render($form);
  if ($js) {
    $commands = array();
    if ($output === FALSE || !empty($form_state['complete'])) {
      $commands[] = ajax_command_html($form_state['object']->idWidget, render(fpeditor_add_content_get_result($object)));
      $commands[] = ctools_modal_command_dismiss();
    }
    elseif (!empty($form_state['cancel'])) {
      $commands[] = ctools_modal_command_dismiss();
    }
    else {
      $commands = ctools_modal_form_render($form_state, $output);
    }
    print ajax_render($commands);
  }
  else {
    if ($output === FALSE || !empty($form_state['complete'])) {
      return render(fpeditor_add_content_get_result($object)) . "\n\r" . l(t('Back'), 'fpeditor');
    }
    elseif (!empty($form_state['cancel'])) {
      drupal_goto('fpeditor');
    }
    else {
      return $output;
    }
  }
}

function fpeditor_autocomplente_node($string){
$matches = array();
  $result = db_select('node', 'n')
    ->fields('n', array('title','nid'))
    ->condition('title', '%' . db_like($string) . '%', 'LIKE')
    ->execute();
  foreach ($result as $row) {
    $matches['('.$row->nid.') '.$row->title] = check_plain($row->title);
  }
  drupal_json_output($matches);
}















