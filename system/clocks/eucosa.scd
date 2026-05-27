
// run eucosa on the server, connected to the clockbus,
// but send messages to the client whenver a trigger occurs.

(

"loading eucosa".postln;

SynthDef(\eucosaSource, { |clockBus, scopeBus=(-1), length=16, beats=4, thresh0=0.25, thresh1=0.25|
    var trigger0, trigger1, measure, err, isScope;
    #trigger0, trigger1, measure, err = EuCosa.ar(In.ar(clockBus, 1), length, beats, thresh0, thresh1);
    SendReply.ar(HPZ1.ar(trigger0) > 0, '/eucosa', err, replyID: 0);
    SendReply.ar(HPZ1.ar(trigger1) > 0, '/eucosa', err, replyID: 1);
    SendReply.ar(HPZ1.ar(measure) > 0,  '/eucosa', err, replyID: 2);

	isScope = scopeBus >= 0;
	Out.ar(bus: scopeBus, channelsArray: trigger0 * isScope);

    Silent.ar(1); // keep from gc
}).add;

"eucosa loaded".postln
)

