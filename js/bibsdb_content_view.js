(function($) {
  'use strict';

 /**
  * Display usage details when the the usage count button is clicked.
  */
  function usage_button_click() {

    $( ".bibsdb-node-usage .usage-button" ).click(function() {
      //Display the usage-details of the node that has been clicked
      $(this).siblings().show();
    });
  }

  // When ready start the magic.
  $(document).ready(function () {
    usage_button_click();
  });

})(jQuery);