
$(function(){
  $(window).on('hashchange',function(){ 
    hashchanged();
  });
    hashchanged();

});

function hashchanged(){
  var hash = location.hash.replace( /^#/, '' );
  if(hash == ""){
    $("#mainView").load("home.html");
  }else{
    $("#mainView").load(hash + ".html");
    
  }
  document.body.scrollTop = document.documentElement.scrollTop = 0;
}
