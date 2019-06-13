
Link : UGen {
  classvar <quantum = 4;

	*enable { | toBeEnabled, newQuantum = 4 |
    quantum = newQuantum;
    { FreeSelfWhenDone.kr( this.new1('control', toBeEnabled.asInteger, quantum) ); }.play;
	}
}


LinkTempo : UGen {
	*kr {
		^this.multiNew('control');
	}
}

LinkBeat : UGen {
  *kr {
		^this.multiNew('control');
	}
}

/*LinkPhase : UGen {
  *kr {
		^this.multiNew('control');
	}
}*/

LinkCount : UGen {
  *kr {| rate = 1 |
		^this.multiNew('control', rate);
	}
}

LinkTrig : UGen {
	*kr { | rate = 1 |
		var count = LinkCount.kr(rate);
		^Changed.kr(count - Latch.kr(count,1));
	}
}

