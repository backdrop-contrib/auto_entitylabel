/**
 * @file
 * JavaScript behaviors for admin pages.
 */

(function ($) {

  Backdrop.behaviors.auto_entitylabelFieldsetSummaries = {
    attach: function (context) {
      function checkPatternLabel(option) {
        if (option === '0') {
          $('.pattern-label').attr("disabled", "disabled");
          $('.pattern-label').attr("readonly", "readonly");
        }
        else {
          $('.pattern-label').removeAttr("disabled");
          $('.pattern-label').removeAttr("readonly");
        }
      }

      var option = $('input[name=auto_entitylabel_status]:checked', '#edit-auto-entitylabel-status').val();

      checkPatternLabel(option);

      $("#edit-auto-entitylabel-status input").change(function () {
        option = $('input[name=auto_entitylabel_status]:checked', '#edit-auto-entitylabel-status').val();
        checkPatternLabel(option);
      });
    }
  };

})(jQuery);
