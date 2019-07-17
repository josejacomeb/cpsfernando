var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res) {
    console.log("prelogin");
  res.sendfile(__dirname + "/public/index.html");
  console.log("index");
  console.log(path.join(__dirname, 'public'));
});

module.exports = router;
