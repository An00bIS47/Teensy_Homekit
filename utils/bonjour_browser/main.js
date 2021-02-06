var bonjour = require('bonjour')()

// browse for all http services
var browser = bonjour.find({ type: 'hap' }, function (service) {
  console.log('Found an HTTP server:', service)
})