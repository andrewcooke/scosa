
Justo : MultiOutUGen {
    
    *ar { |trigger, maxSize = 32, root = 440, maxDistance = 200, mutate = 0, reset = 0, reverse = 0, numerator = 1, denominator = 1, seed = 0|
        var out;
		var expand = { |input|
			if (input.rate == \scalar) {
				if (input < 1.0) {
					(TRand.ar(0.0, 1.0, trigger) < input).asAudioRateInput;
				} {
					(((PulseCount.ar(trigger) - 1) % input.asInteger) < 0.5).asAudioRateInput;
				};
			} {
				if (input.rate != \audio) { K2A.ar(input) } { input };
			};
		};
		mutate = expand.(mutate);
		reset = expand.(reset);
		reverse = expand.(reverse);
		if (numerator.rate != \audio) {numerator = K2A.ar(numerator)};
		if (denominator.rate != \audio) {denominator = K2A.ar(denominator)};
		out = this.multiNew('audio', trigger, maxSize, root, maxDistance, mutate, reset, reverse, numerator, denominator, seed);
		^(frequency: out[0], numerator: out[1], denominator: out[2], distance: out[3])
    }

    init { |... theInputs|
        inputs = theInputs;
        ^this.initOutputs(4, rate);
    }
    
    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"Justo trigger input must be audio rate (ar).";
        };
		(1..3).do { |i|
			if (inputs[i].rate != 'scalar') {
				^"Justo input " ++ i ++ " must be scalar.";
			};
		};
		(4..8).do { |i|
			if (inputs[i].rate != 'audio') {
				^"Justo input " ++ i ++ " must be audio rate (ar).";
			};
        };
        if (inputs[9].rate != 'scalar') {
            ^"Justo seed input must be scalar.";
        };
        ^this.checkValidInputs
    }

}
