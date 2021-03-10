`timescale 1ns / 1ps

module servo_TB;

	// Inputs
	reg	clk;
	reg [7:0] dutty;

	// Outputs

	wire	PWM3;
    wire   CLKOUT3;

	// Instantiate the Unit Under Test (UUT)
	servo uut (
	    .clk(clk),
		.dutty(dutty),
        .CLKOUT3(CLKOUT3),
		.PWM3(PWM3)
	);
	
	always  // Reloj de 50MHz      Periodo=20ns
	begin
		clk =1'b0;
		#10;
		clk =1'b1;
		#10;
	end

	initial
	begin
		//Initialize Inputs
		dutty= 0;
		#40000000;
        dutty=8'b11111110; //Izquierda //0 grados       //10110100   8'b01011010
	    #40000000;
	    dutty=8'b11111101; //Centro    //90 grados
        #40000000;
        dutty=8'b11111011; //Derecha  //180 grados
        #40000000;
        dutty=8'b11110111; //Aletorio //O grados
        #40000000;
	end


	initial
	begin: TEST_CASE
		     $dumpfile("servo_TB.vcd");
		     $dumpvars(-1, uut);
		     #(300000000) $finish;
	end
      
endmodule

