EuCosa : UGen {
    
    *ar { |trig = 0.0, length = 16.0, beats = 4.0, thresh = 0.25|
        ^this.multiNew('audio', trig, length, beats, thresh)
    }
	
    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"EuCosa trigger input must be audio rate (ar).";
        };
		(1..3).do { |i|
			if (inputs[i].rate != 'control') {
				^"EuCosa input " ++ i ++ " must be control rate (kr).";
			};
		}
        ^this.checkValidInputs
    }
	
    name { ^"EuCosa" }
}
