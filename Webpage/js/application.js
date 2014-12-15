
$(function(){
  $(window).on('hashchange',function(){ 
    hashchanged();
  });
    hashchanged();

});

function hashchanged(){
  var hash = location.hash.replace( /^#/, '' );
    $(".active").toggleClass("active");
  if(hash == ""){
    $("#mainView").load("home.html");
    $(".home").toggleClass("active");
  }else{
    $("#mainView").load(hash + ".html");
    $("." + hash + "").toggleClass("active");
    
  }
  document.body.scrollTop = document.documentElement.scrollTop = 0;
}
