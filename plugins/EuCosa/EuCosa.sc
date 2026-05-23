EuCosa : UGen {
    
    *ar { |trig = 0.0, divider = 1.0|
        ^this.multiNew('audio', trig, divider)
    }
	
    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"EuCosa trigger input must be audio rate (ar).";
        };
        if (inputs[1].rate != 'control') {
            ^"EuCosa divider input must be control rate (kr).";
        };
        ^this.checkValidInputs
    }
	
    name { ^"EuCosa" }
}
