
Justo : UGen {
    
    *ar { |trigger, maxSize = 32, root = 440|
        ^this.multiNew('audio', trigger, maxSize, root)
    }

    checkInputs {
		var rate2 = inputs[2].rate;
        if (inputs[0].rate != 'audio') {
            ^"Justo trigger input must be audio rate (ar).";
        };
        if (inputs[1].rate != 'scalar') {
            ^"Justo maxSize input must be scalar.";
        };
		if (rate2 != 'control' and: { rate2 != 'scalar' }) {
			^"Justo root input must be control rate (kr) or scalar, not " ++ rate2;
		};
        ^this.checkValidInputs
    }

}
