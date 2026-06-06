
Justo : UGen {
    
    *ar { |trigger, maxSize = 32, mutate, root = 440|
        ^this.multiNew('audio', trigger, maxSize, mutate, root)
    }

    checkInputs {
		var rate3 = inputs[3].rate;
        if (inputs[0].rate != 'audio') {
            ^"Justo trigger input must be audio rate (ar).";
        };
        if (inputs[1].rate != 'scalar') {
            ^"Justo maxSize input must be scalar.";
        };
        if (inputs[2].rate != 'audio') {
            ^"Justo mutate input must be audio rate (ar).";
        };
		if (rate3 != 'control' and: { rate3 != 'scalar' }) {
			^"Justo root input must be control rate (kr) or scalar, not " ++ rate3;
		};
        ^this.checkValidInputs
    }

}
