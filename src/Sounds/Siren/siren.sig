AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 17
   name "Mode0"
   tl 230 -222.5
   children {
    20
   }
   value 1
  }
  IOPItemInputClass {
   id 22
   name "Mode1"
   tl 228.75 -136.25
   children {
    34
   }
  }
  IOPItemInputClass {
   id 23
   name "Mode2"
   tl 233.75 -41.25
   children {
    35
   }
  }
  IOPItemInputClass {
   id 25
   name "Mode3"
   tl 246.417 46.417
   children {
    36
   }
  }
  IOPItemInputClass {
   id 28
   name "PitchVariance"
   tl 264 206
   children {
    27
   }
  }
  IOPItemInputClass {
   id 30
   name "VolumeVariance"
   tl 256 127
   children {
    34 35 36
   }
  }
 }
 Ops {
  IOPItemOpMulClass {
   id 34
   name "Mul 34"
   tl 427 -130
   children {
    18
   }
   inputs {
    ConnectionClass connection {
     id 30
     port 0
    }
    ConnectionClass connection {
     id 22
     port 0
    }
   }
  }
  IOPItemOpMulClass {
   id 35
   name "Mul 34"
   tl 433 -44
   children {
    21
   }
   inputs {
    ConnectionClass connection {
     id 30
     port 0
    }
    ConnectionClass connection {
     id 23
     port 0
    }
   }
  }
  IOPItemOpMulClass {
   id 36
   name "Mul 34"
   tl 441 49
   children {
    24
   }
   inputs {
    ConnectionClass connection {
     id 30
     port 0
    }
    ConnectionClass connection {
     id 25
     port 0
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 18
   name "SirenOnce"
   tl 630.75 -137
   input 34
  }
  IOPItemOutputClass {
   id 20
   name "Horn"
   tl 621 -224.75
   input 17
  }
  IOPItemOutputClass {
   id 21
   name "SirenSlow"
   tl 618.25 -41.75
   input 35
  }
  IOPItemOutputClass {
   id 24
   name "SirenFast"
   tl 642.917 66.917
   input 36
  }
  IOPItemOutputClass {
   id 27
   name "PitchVariance"
   tl 648 163
   input 28
  }
 }
 compiled IOPCompiledClass {
  visited {
   645 517 518 389 267 390 261 139 262 133 11 6 5 134
  }
  ins {
   IOPCompiledIn {
    data {
     1 65538
    }
   }
   IOPCompiledIn {
    data {
     1 3
    }
   }
   IOPCompiledIn {
    data {
     1 65539
    }
   }
   IOPCompiledIn {
    data {
     1 131075
    }
   }
   IOPCompiledIn {
    data {
     1 262146
    }
   }
   IOPCompiledIn {
    data {
     3 3 65539 131075
    }
   }
  }
  ops {
   IOPCompiledOp {
    data {
     1 2 4 327680 0 65536 0
    }
   }
   IOPCompiledOp {
    data {
     1 131074 4 327680 0 131072 0
    }
   }
   IOPCompiledOp {
    data {
     1 196610 4 327680 0 196608 0
    }
   }
  }
  outs {
   IOPCompiledOut {
    data {
     0
    }
   }
   IOPCompiledOut {
    data {
     0
    }
   }
   IOPCompiledOut {
    data {
     0
    }
   }
   IOPCompiledOut {
    data {
     0
    }
   }
   IOPCompiledOut {
    data {
     0
    }
   }
  }
  processed 14
  version 2
 }
}