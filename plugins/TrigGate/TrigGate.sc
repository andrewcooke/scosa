
TrigGate : MultiOutUGen {
    
    *ar { |fast = 0.0, slow = 0.0, length = 3.0|
        ^this.multiNew('audio', fast, slow, length)
    }

    init { |... theInputs|
        inputs = theInputs;
        ^this.initOutputs(3, rate);
    }
    
    checkInputs {
        (0..2).do { |i|
            var rate = inputs[i].rate;
            if (rate != 'audio') {
                ^"TrigGate input " ++ i ++ " must be audio rate (ar).";
            };
        };
        if (inputs[3].rate != 'control' and: { inputs[3].rate != 'scalar' }) {
            ^"TrigGate length input must be control rate (kr) or scalar.";
        };
        ^this.checkValidInputs
    }

    name { ^"TrigGate" }
}
