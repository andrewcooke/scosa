EuCosa : UGen {
    
    *ar { |trigger = 0.0, length = 16.0, beats = 4.0, thresh0 = 0.25, thresh1 = 0.25|
        ^this.multiNew('audio', trigger, length, beats, thresh0, thresh1)
    }

	 initOutputs { |numChannels, rate|
        channels = Array.fill(3, { 
            OutputProxy(rate, this, channels.size) 
        });
        ^this
    }

    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"EuCosa trigger input must be audio rate (ar).";
        };
		(1..4).do { |i|
			if (inputs[i].rate != 'control') {
				^"EuCosa input " ++ i ++ " must be control rate (kr).";
			};
		}
        ^this.checkValidInputs
    }
	
    name { ^"EuCosa" }
}
