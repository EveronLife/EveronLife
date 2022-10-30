AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 1
   name "HornState"
   tl -167 112
   children {
    2 4
   }
  }
  IOPInputValueClass {
   id 5
   name "Value 5"
   tl -151 -2
   children {
    4
   }
   value 1
  }
 }
 Ops {
  IOPItemOpSubClass {
   id 4
   name "Sub 4"
   tl 38 -3
   children {
    3
   }
   inputs {
    ConnectionClass connection {
     id 5
     port 0
    }
    ConnectionClass connection {
     id 1
     port 1
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 2
   name "SirenState"
   tl 213 110
   input 1
  }
  IOPItemOutputClass {
   id 3
   name "SirenStateInv"
   tl 220 6
   input 4
  }
 }
 compiled IOPCompiledClass {
  visited {
   133 5 7 134 6
  }
  ins {
   IOPCompiledIn {
    data {
     2 2 3
    }
   }
   IOPCompiledIn {
    data {
     1 3
    }
   }
  }
  ops {
   IOPCompiledOp {
    data {
     1 65538 4 65536 0 0 1
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
  }
  processed 5
  version 2
 }
}