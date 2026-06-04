
Justo : UGen {
    
    *ar { |trigger|
        ^this.multiNew('audio', trigger)
    }

    checkInputs {
        if (inputs[0].rate != 'audio') {
            ^"Justo trigger input must be audio rate (ar).";
        };
        ^this.checkValidInputs
    }

}
