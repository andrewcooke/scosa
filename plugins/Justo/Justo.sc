
Justo : MultiOutUGen {
    
    *ar { |trigger, maxSize = 32, root = 440, mutate, numerator, denominator|
        var out = this.multiNew('audio', trigger, maxSize, root, mutate, numerator, denominator);
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
        if (inputs[1].rate != 'scalar') {
            ^"Justo maxSize input must be scalar.";
        };
        if (inputs[2].rate != 'scalar') {
            ^"Justo root input must be scalar.";
        };
		(3..5).do { |i|
			if (inputs[i].rate != 'audio') {
				^"Justo input " ++ i ++ " must be audio rate (ar).";
			};
        };
        ^this.checkValidInputs
    }

}
