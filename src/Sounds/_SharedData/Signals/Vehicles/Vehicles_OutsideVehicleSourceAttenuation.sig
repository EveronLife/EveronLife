AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 30
   name "GIsThirdPersonCam"
   tl 2549.065 -2470.621
   children {
    33
   }
   global 1
  }
  IOPInputValueClass {
   id 31
   name "1"
   tl 2548.759 -2611.065
   children {
    33
   }
   value 1
  }
  IOPItemInputClass {
   id 32
   name "GCurrVehicleCoverage"
   tl 3002.149 -2306.843
   children {
    37
   }
   value 1
   global 1
  }
  IOPItemInputClass {
   id 45
   name "UnderPlayerControl"
   tl 2549.464 -2850.536
   children {
    46
   }
   value 1
  }
  IOPInputValueClass {
   id 48
   name "1"
   tl 3295.714 -3065.714
   children {
    49
   }
   value 1
  }
  IOPInputValueClass {
   id 51
   name "1"
   tl 3409.107 -2283.75
   children {
    52
   }
   value 1
  }
 }
 Ops {
  IOPItemOpSubClass {
   id 33
   name "Sub 33"
   tl 2769.759 -2559.51
   children {
    37 43 46
   }
   inputs {
    ConnectionClass connection {
     id 30
     port 1
    }
    ConnectionClass connection {
     id 31
     port 0
    }
   }
   Subtracter 0
  }
  IOPItemOpMulClass {
   id 37
   name "Mul 37"
   tl 3209.371 -2427.288
   children {
    39
   }
   inputs {
    ConnectionClass connection {
     id 33
     port 0
    }
    ConnectionClass connection {
     id 32
     port 0
    }
   }
  }
  IOPItemOpSmootherClass {
   id 39
   name "Smoother 39"
   tl 3415.833 -2422.083
   children {
    2 52
   }
   inputs {
    ConnectionClass connection {
     id 37
     port 0
    }
   }
   "Fade In Time" 400
   "Fade Out Time" 400
  }
  IOPItemOpInterpolateClass {
   id 44
   name "Interpolate 44"
   tl 3585.357 -2855.893
   children {
    40
   }
   inputs {
    ConnectionClass connection {
     id 46
     port 0
    }
   }
   "Y min" 1
   "Y max" 0.7
  }
  IOPItemOpMulClass {
   id 46
   name "Mul 46"
   tl 3286.25 -2856.25
   children {
    44 49
   }
   inputs {
    ConnectionClass connection {
     id 33
     port 0
    }
    ConnectionClass connection {
     id 45
     port 0
    }
   }
  }
  IOPItemOpSubClass {
   id 49
   name "Sub 49"
   tl 3511.429 -3037.143
   children {
    47
   }
   inputs {
    ConnectionClass connection {
     id 46
     port 1
    }
    ConnectionClass connection {
     id 48
     port 0
    }
   }
   Subtracter 0
  }
  IOPItemOpSubClass {
   id 52
   name "Sub 52"
   tl 3621.036 -2283.322
   children {
    50
   }
   inputs {
    ConnectionClass connection {
     id 39
     port 1
    }
    ConnectionClass connection {
     id 51
     port 0
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 2
   name "InVehicle_W"
   tl 3838.009 -2421.329
   input 39
  }
  IOPItemOutputClass {
   id 40
   name "InVehEngine_S"
   tl 3831.964 -2857.917
   input 44
  }
  IOPItemOutputClass {
   id 43
   name "InVeh_V"
   tl 3838.571 -2558.572
   input 33
  }
  IOPItemOutputClass {
   id 47
   name "InVeh_S"
   tl 3829.286 -3040.714
   input 49
  }
  IOPItemOutputClass {
   id 50
   name "OutsideVehicle_W"
   tl 3831.25 -2284.107
   input 52
  }
 }
 compiled IOPCompiledClass {
  visited {
   645 517 389 261 133 5 7 523 647 390 391 134 262 139 263 775 518 6
  }
  ins {
   IOPCompiledIn {
    data {
     1 3
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
     1 262147
    }
   }
   IOPCompiledIn {
    data {
     1 327683
    }
   }
   IOPCompiledIn {
    data {
     1 393219
    }
   }
  }
  ops {
   IOPCompiledOp {
    data {
     3 65539 131074 262147 4 0 1 65536 0
    }
   }
   IOPCompiledOp {
    data {
     1 131075 4 1 0 131072 0
    }
   }
   IOPCompiledOp {
    data {
     2 2 393219 2 65537 0
    }
   }
   IOPCompiledOp {
    data {
     1 65538 2 262145 0
    }
   }
   IOPCompiledOp {
    data {
     2 196611 327683 4 1 0 196608 0
    }
   }
   IOPCompiledOp {
    data {
     1 196610 4 262145 1 262144 0
    }
   }
   IOPCompiledOp {
    data {
     1 262146 4 131073 1 327680 0
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
  processed 18
  version 2
  ops_reeval_list {
   2
  }
 }
}