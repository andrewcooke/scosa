
EuCosa : MultiOutUGen {
    
    *ar { |trigger = 0.0, length = 16.0, reference = 4.0, beats = 4.0, thresh0 = 0.25, thresh1 = 0.25|
        ^this.multiNew('audio', trigger, length, reference, beats, thresh0, thresh1)
    }

    init { |... theInputs|
        inputs = theInputs;
        ^this.initOutputs(4, rate);
    }
    
    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"EuCosa trigger input must be audio rate (ar).";
        };
        (1..5).do { |i|
            var rate = inputs[i].rate;
            if (rate != 'control' and: { rate != 'scalar' }) {
                ^"EuCosa input " ++ i ++ " must be control rate (kr) or scalar.";
            };
        };
        ^this.checkValidInputs
    }

}
