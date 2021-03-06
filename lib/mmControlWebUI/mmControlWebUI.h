/**
 * Web interface for mmControl project.
 */
#include <Arduino.h>

const char index_html[] PROGMEM =
    "<html><head>%REFRESHINDEXTAG%<link rel=\"icon\" type=\"image/vnd.microsoft.icon\" href=\"/favicon.ico\">%CSS_STYLE%</head>\n\
<body style='height:100%%;'><div style='position:relative;min-height:95%%;'>\
<div style='background-color:lightgrey;position:absolute;width:98%%;bottom:0;text-align:center;height:2.5em;margin-left:1%%;'>\
%REFRESHINDEXLINK% &nbsp; | &nbsp; <a href=\"/manual.html\">Hilfe</a> &nbsp; | &nbsp; <a href=\"/log.html\">Log</a> <span style=\"white-space:nowrap;\">Build: %__TIMESTAMP__%</span></div>\
<h1 style='position: absolute;top: 0;padding-left:10%%;'>%APPNAME%</h1>\
<div style='padding:1em 1%% 2em 1%%;'><p style=\"padding-top:10%%;\">%STATUSBAR%Ger&auml;t aktivieren:<div style=\"text-align: center;\">%BUTTONS%</div></p>\
<div style=\"margin-top:50px;background-color: rgb(230,230,230);padding:5px;border-top:solid 1px;\">%USERMESSAGE%\
TV-Quelle: <button onclick=\"location.href='/channel/up';\" class=\"button2\">Channel Up</button> \
<button onclick=\"location.href='/channel/down';\" class=\"button2\">Channel Down</button> &nbsp; %BUTTON_MAIN%<br/>\
Lautst�rke: <button onclick=\"location.href='/volume/up';\" class=\"button\">+</button> \
<button onclick=\"location.href='/volume/down';\" class=\"button\">&ndash;</button> <br/>\
<p style=\"margin-top:20px;\"><span style=\"text-decoration:underline;\">IST-Zustand:</span><br/>Quelle = <b>%ACTIVE_CHANNEL%</b>, korrigiere zu:  %SETCHANNEL_LINKS% \
<a href=\"/device/main/on\">AN</a> <a href=\"/device/main/off\">AUS</a> &nbsp; &nbsp;\
<form style=\"padding-bottom:15px;\" method=\"get\" action=\"/index.html\">Volume = <input type=\"text\" name=\"knownVolume\" value=\"%VOLUME_CURRENT%\" style=\"width:40px;\"/><input type=\"submit\" value=\"Ok\"/></form>\
</div></div></div></body></html>";

// font-size:5vw;
const char css_style[] PROGMEM =
    "<style>*{margin:0;font-family:Arial,Helvetica,sans-serif;} h1{font-size:5vw;} h2{padding-top:15px;} p{margin:15px 25px 0px 5px;}\
 .active{background-color:green;} .off {color:red;} .startup{color:green;} .shutdown {color:blue;} .button{margin-bottom:10px;font-size:5vw;} .button2{margin-bottom:10px;font-size:3vw;}</style>";

const char log_html[] PROGMEM =
    "<html><head>%REFRESHLOGTAG%<link rel=\"icon\" type=\"image/vnd.microsoft.icon\" href=\"/favicon.ico\"><style>\
 .log{font-family:\"Consolas\",\"lucida console\",monospace;padding:5px;font-size:15px;word-break:break-all;word-wrap:break-word;color:white;background-color:#111;display:block;line-height:1.5em;margin-top:0;}\
 .head{position:fixed;height:2em;width:95%%;background-color:lightgrey;margin:0 2%% 0 2%%;font-family: Arial, Helvetica, sans-serif;font-size:2vw;}\
 .active{background-color:green;} .shutdown{color:blue;}</style>\
</head>\n<body style=\"background-color:grey;\"><table class=\"head\"><tr><td style = \"text-align: left;\">\
<a href=\"/index.html\">Back</a> &nbsp; %REFRESHLOGLINK% &nbsp; %IRRCV_BUTTON% <a href=\"/sleep/now\">Sleep</a> <a href=\"/log/memory\">Mem</a></td>\
<td style=\"text-align:center;\">Log - Protokoll  <span style=\"color:blue;font-weight:bold;\">%STATUS%%USERMESSAGE%</span><td>\
<td style=\"text-align:right;padding-right:5px;\" title=\"%UPSINCE% / %RESET_REASON%\">Now = %SYSTIME%</td></tr></table>\
<div style=\"padding-top:60px;\"><pre class=\"log\">%LOG0%%LOG1%</pre></div><script>if(window.location.hash){window.scrollTo(0, document.body.scrollHeight);}</script></body></html>";

const char irRcvOff_html[] PROGMEM = "<html><head><meta http-equiv=\"refresh\" content=\"3;url=/log.html\"><link rel=\"icon\" type=\"image/vnd.microsoft.icon\" href=\"/favicon.ico\"></head>\n<body>Please wait a minute...</body></html>";

const char manual_html[] PROGMEM = "<html><head><style> body{font-family:\"Lucida Sans Unicode\",\"Lucida Grande\",sans-serif;}</style>%CSS_STYLE%</head><body><h1>mmControl: Bedienungsanleitung</h1>%MANUAL_TEXT% <p><a href=\"/index.html\">Zur&uuml;ck</a></p></body></html>";
const char manual_txt_de[] PROGMEM =
"<h2>Alexa-Sprachbefehle</h2>\
<ul>%VOICE_COMMANDS%</ul>\
<h2>Bedienung via Web-Oberfl&auml;che</h2>\
<p>Klicke auf einen Button, um das jeweilige Ger�t zu aktivieren und den Fernseher auf dessen Eingang umzuschalten.\
<br/>Die Farben der Buttons haben folgende Bedeutung:</p><ul>\
<li><span class='off'>ausgeschaltet</span> (f�r Firestick/Playstation/DVD: Stromversorgung unterbrochen)</li>\
<li><span class='active'>eingeschaltet</span></li>\
<li><span class='startup'>einschalten l�uft</span></li>\
<li><span class='shutdown'>ausschalten l�uft</span></li></ul>\
<p>Beim Ausschalten ist eine Verz�gerung realisiert, w�hrend dieser Zeit kann das Ger�t direkt wieder eingeschaltet und weiter genutzt werden.</p>\
<p>Mit der Auswahl in TV-Kanal im mittleren Bereich kann der Eingang des Fernsehers direkt umgeschaltet werden. Der Ger�testatus wird davon nicht beeinflusst.</p>\
<p>Mit Klick auf einen Link im Block \"Aktiv\" bekommt mmControl die Information, welcher Eingangskanal des Fernsehers gerade aktiv ist. \
Dies ist f�r den Abgleich bei manuellen Umschaltungen hilfreich.</p>";

const String PARAM_REFRESH = "r";
const String PARAM_VERBOSE = "v";
const String PARAM_DEVICEID = "d";

#define favicon_ico_len 524
const byte favicon_ico_gzip[] PROGMEM = {
    // clang-format off
    0x1f,0x8b,0x8,0x8,0xc9,0xdb,0x75,0x5e,0x0,0x3,0x69,0x63,0x6f,0x6e,0x73,0x38,
    0x5f,0x66,0x65,0x72,0x6e,0x62,0x65,0x64,0x69,0x65,0x6e,0x75,0x6e,0x67,0x5f,0x32,
    0x34,0x5f,0x73,0x61,0x33,0x2e,0x69,0x63,0x6f,0x0,0xad,0x95,0xcd,0x2b,0x44,0x51,
    0x18,0xc6,0x9f,0x31,0x32,0x14,0x23,0x89,0x2c,0xe4,0x63,0x67,0x69,0x85,0xb2,0x60,
    0x6f,0xa3,0xf0,0x67,0xf0,0x17,0x88,0xf1,0xad,0x24,0xa5,0x14,0xa5,0xc8,0x47,0x61,
    0x63,0x69,0xec,0xd8,0x89,0x8d,0xec,0x15,0xb,0xfb,0xc9,0x50,0x42,0x8e,0xe7,0x75,
    0xdf,0x9b,0xd3,0x9d,0x7b,0xc7,0x3d,0x63,0xde,0xe9,0x77,0xe7,0xde,0xf7,0xdc,0xf3,
    0x3c,0xe7,0xbc,0xe7,0x74,0x2e,0x90,0xe0,0xaf,0xa5,0x5,0xbc,0x76,0x60,0xb9,0x6,
    0x68,0x6,0xd0,0x45,0x98,0x42,0x37,0xbc,0xfc,0x4f,0xd4,0xc0,0x25,0x46,0xc9,0x1,
    0x49,0x3a,0xf5,0x8a,0x17,0x23,0xe4,0x9d,0x18,0x72,0x5c,0x66,0xf,0x5b,0xdb,0x67,
    0x97,0x54,0x94,0x41,0x5b,0x6a,0xf2,0xa1,0x9a,0x53,0xa4,0x8e,0x4c,0xeb,0xf3,0x26,
    0xa4,0xd4,0xe5,0xd1,0x36,0xaa,0x2d,0x91,0xb6,0x72,0xeb,0x25,0x7a,0x4,0xb5,0x85,
    0x49,0x52,0xb,0x6f,0x1e,0x76,0x7e,0xf5,0x1f,0xda,0xc1,0x9a,0x98,0x88,0xfc,0xa2,
    0x83,0xbe,0xad,0x15,0x56,0x93,0xa8,0xfc,0x9c,0x83,0xc7,0x3c,0x8a,0xd7,0x24,0x2a,
    0x3f,0xeb,0xe0,0xb1,0x8c,0xe2,0x35,0x89,0xc2,0xc5,0x63,0x5,0xc5,0x6b,0x92,0x25,
    0xbd,0xda,0xd6,0x47,0xce,0x35,0x3f,0xe3,0xe0,0xb1,0x86,0xf0,0x9a,0x88,0x76,0x70,
    0x6f,0x26,0x2c,0x8f,0x4c,0x4c,0x7d,0xe9,0xb3,0x8e,0xc2,0x3a,0xf4,0x68,0xfb,0x2,
    0xc9,0xc3,0x5b,0x33,0xe8,0x3c,0xfc,0x77,0x5c,0x3c,0x36,0x2,0xfa,0x7e,0xbd,0xf2,
    0xfa,0xfc,0xac,0xcf,0x75,0x81,0xf7,0x6,0x1d,0x3c,0xb6,0x42,0xc6,0x2f,0x7b,0xff,
    0x45,0xe7,0x21,0xd1,0x5b,0xc2,0xf8,0xfd,0x90,0x33,0x6d,0x5b,0xfb,0x9e,0x21,0xbc,
    0xfe,0xd9,0x22,0xda,0x55,0x31,0x3d,0x76,0x2c,0xf,0x99,0x47,0xad,0x8e,0x3b,0x4a,
    0xbb,0x15,0xde,0xba,0x5f,0xc7,0xd0,0xf7,0x3d,0xf6,0x10,0xbe,0xff,0x7d,0xed,0x6a,
    0x72,0x42,0xf6,0xe1,0x7d,0xe2,0xa4,0xed,0x25,0xa6,0xbe,0x84,0x7c,0x63,0xe,0x23,
    0xb4,0xe5,0xcb,0xd8,0xaf,0xb9,0x27,0xfc,0xae,0xf9,0x73,0x81,0xca,0xdf,0x1e,0x47,
    0x28,0xac,0xc9,0xa9,0xe5,0x29,0xeb,0x3f,0xac,0xf7,0x57,0x8e,0xfa,0x12,0x95,0x64,
    0x2c,0x90,0x13,0x4f,0x19,0xf7,0x12,0x69,0x24,0xf7,0xaa,0x3f,0x5e,0x82,0x7e,0x30,
    0x64,0x2d,0x9b,0xe0,0xad,0xf9,0x10,0xb9,0x53,0xed,0x5b,0x92,0x2a,0x83,0xbe,0x7f,
    0x3e,0xd8,0xdc,0xaa,0xef,0x7f,0x43,0xf6,0xf7,0xd,0x79,0x85,0xb7,0x57,0xa4,0xde,
    0x13,0xb0,0xc6,0xfd,0x39,0x6d,0xf0,0x36,0x60,0x90,0x6b,0x37,0x78,0xa8,0x37,0xb8,
    0x4c,0x19,0x64,0x8,0x92,0x6,0xd5,0x9d,0xbc,0xe7,0x70,0x2e,0x78,0x44,0x3f,0x62,
    0x0,0xb9,0x74,0x3b,0x72,0xd,0xf5,0x78,0x6b,0x48,0xe1,0x93,0xff,0x5f,0xe9,0x36,
    0xb6,0xf6,0xc1,0x64,0x6,0x61,0x2e,0xf8,0xe6,0x23,0xc9,0x13,0x76,0x37,0x26,0x1,
    0xf3,0x95,0xc4,0x37,0xd3,0xf6,0x1d,0x3c,0x9e,0x9,0x0,0x0,
    // clang-format on
};
