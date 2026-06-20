
Justo : MultiOutUGen {
    
    *ar { |trigger, maxSize = 32, root = 440, maxDistance = 200, mutate, reset, numerator, denominator, seed = 0|
        var out = this.multiNew('audio', trigger, maxSize, root, maxDistance, mutate, reset, numerator, denominator, seed);
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
		(4..7).do { |i|
			if (inputs[i].rate != 'audio') {
				^"Justo input " ++ i ++ " must be audio rate (ar).";
			};
        };
        if (inputs[8].rate != 'scalar') {
            ^"Justo seed input must be scalar.";
        };
        ^this.checkValidInputs
    }

}
