

// Actualiza os elementos cada X segundos  //
setInterval(function () {

  actualizaTelemetria("192.168.0.100");

}, 1000);




// Acções a serem executadas ao carre3gar a página //
document.addEventListener("DOMContentLoaded", function () {

  // Carrega Imagens SVG
  let svg_bateria = '<svg viewBox="0 0 512 512" ><path d="M480 128h-32V80c0-8.84-7.16-16-16-16h-96c-8.84 0-16 7.16-16 16v48H192V80c0-8.84-7.16-16-16-16H80c-8.84 0-16 7.16-16 16v48H32c-17.67 0-32 14.33-32 32v256c0 17.67 14.33 32 32 32h448c17.67 0 32-14.33 32-32V160c0-17.67-14.33-32-32-32zM192 264c0 4.42-3.58 8-8 8H72c-4.42 0-8-3.58-8-8v-16c0-4.42 3.58-8 8-8h112c4.42 0 8 3.58 8 8v16zm256 0c0 4.42-3.58 8-8 8h-40v40c0 4.42-3.58 8-8 8h-16c-4.42 0-8-3.58-8-8v-40h-40c-4.42 0-8-3.58-8-8v-16c0-4.42 3.58-8 8-8h40v-40c0-4.42 3.58-8 8-8h16c4.42 0 8 3.58 8 8v40h40c4.42 0 8 3.58 8 8v16z"/></svg>';
  document.getElementById("div_tensao").innerHTML = svg_bateria;

  let svg_temperatura = '<svg viewBox="0 0 24 24" ><g><path d="M12,2 C13.7330315,2 15.1492459,3.35645477 15.2448552,5.06545968 L15.25,5.24987066 L15.251,13.202 L15.3310301,13.270935 C16.2565465,14.097507 16.8481697,15.2418033 16.9745652,16.4939066 L16.9936024,16.7457024 L17,17 C17,19.7614237 14.7614237,22 12,22 C9.23857625,22 7,19.7614237 7,17 C7,15.637307 7.54959924,14.3654986 8.48922288,13.4395696 L8.66993395,13.2700735 L8.749,13.202 L8.75,5.25 C8.75,3.57886252 10.0112953,2.20231635 11.6338512,2.02039625 L11.8155761,2.00514479 L12,2 Z M12,3.5 C11.0818266,3.5 10.3288077,4.20711027 10.2558012,5.10650661 L10.25,5.25003944 L10.2495427,13.9444921 L9.94128095,14.1691409 C9.04185425,14.824607 8.5,15.8663631 8.5,17 C8.5,18.9329966 10.0670034,20.5 12,20.5 C13.9329966,20.5 15.5,18.9329966 15.5,17 C15.5,15.9375513 15.0240648,14.955799 14.2238599,14.2971002 L14.0595403,14.1697396 L13.7514995,13.9451154 L13.75,5.25 C13.75,4.28350169 12.9664983,3.5 12,3.5 Z M12,8 C12.4142136,8 12.75,8.33578644 12.75,8.75 L12.7505732,14.6146307 C13.7645539,14.9333735 14.5,15.8808004 14.5,17 C14.5,18.3807119 13.3807119,19.5 12,19.5 C10.6192881,19.5 9.5,18.3807119 9.5,17 C9.5,15.8804347 10.2359268,14.9327543 11.250421,14.6143184 L11.25,8.75 C11.25,8.33578644 11.5857864,8 12,8 Z" id="🎨-Color"></path></g></svg>';
  document.getElementById("div_temperatura").innerHTML = svg_temperatura;

  let svg_humidade = '<svg width="32px" height="32px" viewBox="0 0 32 32" id="icon"><defs><style>.cls-1 {fill: none;}</style></defs><title>humidity--alt</title><path d="M26,12a3.8978,3.8978,0,0,1-4-3.777,3.9017,3.9017,0,0,1,.6533-2.0639L25.17,2.4141a1.0381,1.0381,0,0,1,1.6592,0L29.3154,6.11A3.9693,3.9693,0,0,1,30,8.223,3.8978,3.8978,0,0,1,26,12Zm0-7.2368L24.3438,7.2257A1.89,1.89,0,0,0,24,8.223a2.0139,2.0139,0,0,0,4,0,1.98,1.98,0,0,0-.375-1.0466Z" transform="translate(0)"/><path d="M23.5,30H8.5A6.4962,6.4962,0,0,1,7.2,17.1381a8.9938,8.9938,0,0,1,17.6006,0A6.4964,6.4964,0,0,1,23.5,30ZM16,12a7,7,0,0,0-6.9414,6.1452l-.0991.8122-.8155.064A4.4962,4.4962,0,0,0,8.5,28h15a4.4962,4.4962,0,0,0,.3564-8.9786l-.8154-.064-.0986-.8122A7.0022,7.0022,0,0,0,16,12Z" transform="translate(0)"/><rect id="_Transparent_Rectangle_" data-name="&lt;Transparent Rectangle&gt;" class="cls-1" width="32" height="32"/></svg>';
  document.getElementById("div_humidade").innerHTML = svg_humidade;

  let svg_pressao = '<svg width="32px" height="32px" viewBox="0 0 32 32"><defs><style>.cls-1 {fill: none;}</style></defs><title>pressure--filled</title><path d="M22,30H10V25H6l10-9,10,9H22Z"/><path d="M16,16,6,7h4V2H22V7h4Z"/><rect id="_Transparent_Rectangle_" data-name="&lt;Transparent Rectangle&gt;" class="cls-1" width="32" height="32"/></svg>';
  document.getElementById("div_pressao").innerHTML = svg_pressao;

});




// Actualizar Hora e Data dos dispositivos //
function actualizaTelemetria() {

  let xhttp = new XMLHttpRequest();
  let var_url = "";
  
  var http = new XMLHttpRequest();
  http.open('GET', "/TELEMETRIA", true);

  http.onload = function () {
    
    const objson = JSON.parse(this.responseText);

    document.getElementById("volt").innerHTML = objson.Tensao;
    document.getElementById("temp").innerHTML = objson.Temperatura;
    document.getElementById("hum").innerHTML = objson.Humidade;
    document.getElementById("pres").innerHTML = objson.Pressao;
    
    //console.log(http.responseText);
  } // success case

  http.onerror = function () { console.log(http.responseText); } // failure case
  http.send();

}



