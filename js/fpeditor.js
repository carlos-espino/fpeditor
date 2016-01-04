jQuery(document).ready(function ($) {
	//$("#txtEditor").Editor();
	//$(".editor").jqte(); 
	
	
	function getWidget(level) {
			idWidget = idWidget+1;
			addWidgetLink = getAddWidgetLink(level)
			return $('<div id="grid-stack-item-'+   idWidget    +'"><div contenteditable="true" class="grid-stack-item-content" />' + deleteWidgetLink + addWidgetLink + configWidgetLink + getAddContentLink(idWidget) +'<div/>');
	}
	
	function getAddWidgetLink(level){
			return '<div title="Add widget"  data-level="'+level+'" class="grid-stack-add-widget grid-stack-custom-control"><i class="fa fa-plus-square-o fa-fw"></i></div>';
	}
	
	function getAddContentLink(idWidget){
		return '<div title="Add content" data-idWidget="'+idWidget+'" class="grid-stack-add-content grid-stack-custom-control ctools-use-modal"><i class="fa fa-file-o fa-fw"></i></div>';
	}
	
	function getTools(){
		return '<select><option selected hidden>Select</option><option value="1">Uno</option><option value="2">Dos</option><option value="3">Tres</option></select>'
	}
	
	var idWidget 					= 0;
	var deleteWidgetLink 	= '<div title="Delete Widget" class="grid-stack-delete-widget-link grid-stack-custom-control"><i class="fa fa-minus-square-o fa-fw"></i></div>';
	var configWidgetLink 	= '<div title="Configure Widget" class="grid-stack-config-widget grid-stack-custom-control ctools-use-modal"><i class="fa fa-cog fa-fw"></i></div>';
	
	
	
	var options 					= { 
			float: true, 
			animate: true,
			resizable: {
            handles: 'e, se, s, sw, w, n, ne, nw'
      }  
  };
	$('.grid-stack').gridstack(options);
  
	
	
	/* 
	 * Configure widget function
	 */
	//~ $('.grid-stack').on('click', '.grid-stack-config-widget', function() {
			//~ grid = $('.grid-stack').data('gridstack')
			//~ grid.remove_widget($('#'+$(this).parent().attr("id")),true)
	//~ });	
	
	
	/* 
	 * Remove widget function
	 */
	$('.grid-stack').on('click', '.grid-stack-delete-widget-link', function() {
			grid = $('.grid-stack').data('gridstack')
			grid.remove_widget($('#'+$(this).parent().attr("id")),true)
	});
	
	
	/* 
	 * Add widget function
	 */
	$('.grid-stack').on('click', '.grid-stack-add-widget', function() {
			level = $(this).data('level');
			console.log(level);
			if(level >0){
				$(this).siblings().first().append('<div class="grid-stack"></div>');
				$('.grid-stack').gridstack(options);
				this.grid = $(this).siblings().first().children().first().data('gridstack');
			} else {
				this.grid = $(this).parent().data('gridstack');
			}
			
			level= level + 1;
			this.grid.add_widget(getWidget(level),0, 0, 6, 3,true);
			$('html, body').animate({ scrollTop: $('#grid-stack-item-'+idWidget).offset().top }, 'slow');
			$('.ui-icon').tooltip();
			make_modal_links();
	});
	
	
	$('.grid-stack').bind("DOMSubtreeModified",function(){
		console.log('changed');
		texto = $('.grid-stack').clone()
		texto.find('.ui-resizable-handle').remove()
		texto.find('.grid-stack-custom-control').remove()
		tinyMCE.activeEditor.setContent(texto.html())
	});
	
	

	function make_modal_links(){
		$('.ctools-use-modal:not(.ctools-use-modal-processed)').each(function(i, obj) {
			 // The URL to your form or content to be displayed on the modal. Remember to add "nojs" at the end.
				if($(obj).hasClass('grid-stack-config-widget')){
						var url = 'fpeditor/nojs/add_content';
				} else if($(obj).hasClass('grid-stack-add-content')){
						var url = 'fpeditor/nojs/add_content_modal_form?widget='+$(obj).attr('data-idWidget');
				}
				
				// Create a drupal ajax object
				$(obj).click(Drupal.CTools.Modal.clickAjaxLink);        // This is to pop up the modal as soon as the user clicks the element.
				var element_settings = {};
				element_settings.url = url;        
				element_settings.event = 'click';
				element_settings.progress = { type: 'throbber' };
				var base = url;
				Drupal.ajax[base] = new Drupal.ajax(base, obj, element_settings);
				$(obj).addClass('ctools-use-modal-processed');           // Add a class to flag that this element has already been processed.
			});
	}


// texto = $('.grid-stack').clone()
// texto.find('.ui-resizable-handle').remove()
// texto.find('.grid-stack-custom-control').remove()
// CKEDITOR.instances['edit-content-value'].setData(texto.html())
// tinyMCE.activeEditor.setContent(texto.html())



	

	
	

	
	
	
	
	
});
