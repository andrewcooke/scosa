
Justo : MultiOutUGen {
    
    *ar { |trigger, maxSize = 32, root = 440, mutate|
        var out = this.multiNew('audio', trigger, maxSize, root, mutate);
		^(frequency: out[0], numerator: out[1], denominator: out[2])
    }

    init { |... theInputs|
        inputs = theInputs;
        ^this.initOutputs(3, rate);
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
        if (inputs[3].rate != 'audio') {
            ^"Justo mutate input must be audio rate (ar).";
        };
        ^this.checkValidInputs
    }

}
