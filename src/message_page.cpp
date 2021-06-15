#include <dashboard_pages.h>

const char MESSAGE_HTML_TEMPLATE[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
<head>
    <title>%TITLE%</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-+0n0xVW2eSR5OomGNYDnhzAbDsOXxcvSN1TPprVMTNDbiYZCxYbOOl7+AMvyTG2x" crossorigin="anonymous">
</head>
<body>
<div class="container py-3">
    <header>
        <div class="d-flex flex-column flex-md-row align-items-center pb-3 mb-4 border-bottom">
            <a href="/" class="d-flex align-items-center text-dark text-decoration-none">
                <svg xmlns="http://www.w3.org/2000/svg" width="40" height="32"><path d="M21.88,9.62H10.12a.5.5,0,0,0-.5.5V21.88a.5.5,0,0,0,.5.5h7.75a.47.47,0,0,0,.35-.15l4-4a.47.47,0,0,0,.15-.35V10.12A.5.5,0,0,0,21.88,9.62Zm-.5,8-3.72,3.72h-7V10.62H21.38ZM29,14.17a.5.5,0,0,0,.5-.5.5.5,0,0,0-.5-.5H25.5V9.5H29a.5.5,0,0,0,0-1H25.5V7a.5.5,0,0,0-.5-.5H23.5V3a.5.5,0,0,0-1,0V6.5H18.83V3a.5.5,0,0,0-.5-.5.5.5,0,0,0-.5.5V6.5H14.17V3a.5.5,0,0,0-.5-.5.5.5,0,0,0-.5.5V6.5H9.5V3a.5.5,0,0,0-1,0V6.5H7a.5.5,0,0,0-.5.5V8.5H3a.5.5,0,0,0,0,1H6.5v3.67H3a.5.5,0,0,0-.5.5.5.5,0,0,0,.5.5H6.5v3.66H3a.5.5,0,0,0-.5.5.5.5,0,0,0,.5.5H6.5V22.5H3a.5.5,0,0,0,0,1H6.5V25a.5.5,0,0,0,.5.5H8.5V29a.5.5,0,0,0,1,0V25.5h3.67V29a.5.5,0,0,0,.5.5.5.5,0,0,0,.5-.5V25.5h3.66V29a.5.5,0,0,0,.5.5.5.5,0,0,0,.5-.5V25.5H22.5V29a.5.5,0,0,0,1,0V25.5H25a.5.5,0,0,0,.5-.5V23.5H29a.5.5,0,0,0,0-1H25.5V18.83H29a.5.5,0,0,0,.5-.5.5.5,0,0,0-.5-.5H25.5V14.17ZM24.5"/></svg>
                <span class="fs-4">%TITLE%</span>
            </a>
        </div>
    </header>

    <main>
        <div class="p-3 pb-md-4 mx-auto text-left">
            <p class="fs-5 text-muted">%MESSAGE%</p>
        </div>
    </main>
</div>
<script type="text/javascript">
    function Redirect() {window.location="/";}
    setTimeout('Redirect()', 10000);
</script>
</body>
</html>
)=====";
