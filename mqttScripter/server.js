var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://broker.hivemq.com')
var restify = require('restify'),
fs = require('fs');


var washSingleSide = 25;
var washingSwitchS = 7;
var spinSingleSide = 125;
var spiningSwitchS = 30;


var state = 0;

function sendCommands(){
	switch(state){
		case 0 : 
			client.publish('/athul/washing/machine', 'B')
			client.publish('/athul/washing/machine', 'D')
			client.publish('/athul/washing/machine', 'F')
		break;
		case 1 : 
			state = 2;
			setTimeout(function(){
				client.publish('/athul/washing/machine', 'D')
				client.publish('/athul/washing/machine', 'E')
				client.publish('/athul/washing/machine', 'A')
				setTimeout(function(){
					client.publish('/athul/washing/machine', 'B')
					setTimeout(function(){
						client.publish('/athul/washing/machine', 'F')
						client.publish('/athul/washing/machine', 'A')
						setTimeout(function(){
							client.publish('/athul/washing/machine', 'B')
							if(state == 2)
								state = 1;
							else
								state = state * -1;
						}, washSingleSide * 1000);
					}, washingSwitchS * 1000);
				}, washSingleSide * 1000);	
			}, washingSwitchS * 1000);
		break;
		case 3 : 
			state = 4;
			setTimeout(function(){
				client.publish('/athul/washing/machine', 'C')
				client.publish('/athul/washing/machine', 'E')
				client.publish('/athul/washing/machine', 'A')
				setTimeout(function(){
					client.publish('/athul/washing/machine', 'B')
					setTimeout(function(){
						client.publish('/athul/washing/machine', 'F')
						client.publish('/athul/washing/machine', 'A')
						setTimeout(function(){
							client.publish('/athul/washing/machine', 'B')
							if(state == 4)
								state = 3;
							else
								state = state * -1;
						}, spinSingleSide * 1000);
					}, spiningSwitchS * 1000);
				}, spinSingleSide * 1000);	
			}, spiningSwitchS * 1000);
		break;
		case 5 :
			client.publish('/athul/washing/machine', 'C')
			client.publish('/athul/washing/machine', 'B')
		break;
	}
}

setInterval(sendCommands, 1000);

var server = restify.createServer({
  name: 'WashingMachine',
});

server.listen(9001);

function send(req, res, next) {
	switch(req.params.char){
		case "W" : 
			if (state == 0)
				state = 1;
			else
				state = -1;
		break;
		case "S" : 
			if (state == 0)
				state = 3;
			else
				state = -3;
		break;
		case "D" : 
			if (state == 0)
				state = 5;
		break;
		case "E" : 
				state = 0;
		break;
	}
	var result = req.params.char + " : State is now ";
	if(state < 0)
		result += "scheduled to be ";
	result += state;
	res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
	res.send(result);
	return next();
}

server.get('/', function (req, res, next){
	res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
	res.send("State is " + state);
	return next();
});

server.get('/:char', send);