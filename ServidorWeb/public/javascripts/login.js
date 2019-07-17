var cajapassword = document.getElementById("contraseña");
function Login() {
    var done = 0;
    var usuario = document.login.usuario.value;
    var password = document.login.password.value;
    if (usuario === "b" && password === "2") {
        window.location = "inicio.html";
    } else if (usuario === "a" && password === "1") {
        window.location = "inicio.html";
    } else {
        document.login.usuario.value = "";
        document.login.password.value = "";
        alert("¡Nombre de usuario o contraseña incorrectos!");
    }
}
cajapassword.addEventListener("keypress", function (data) {
    if (data.key === 'Enter') {
        var usuario = document.login.usuario.value;
        var password = document.login.password.value;
        if (usuario === "b" && password === "2") {
            window.location = "inicio.html";
        } else if (usuario === "a" && password === "1") {
            window.location = "inicio.html";
        } else {
            document.login.usuario.value = "";
            document.login.password.value = "";
            alert("¡Nombre de usuario o contraseña incorrectos!");
        }
    }
});