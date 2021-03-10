
module servo( 
    input clk,
    input [7:0]dutty, 
	 //output CLKOUT1,
	 //output CLKOUT2,
	 output CLKOUT3,
    //output PWM1,
	 //output PWM2,
	 output PWM3
	 );
	 
	 
reg [7:0] grados;	 
	 
always @(posedge clk) begin
		  case(dutty) 
	 8'b111111110:   grados=8'b00000000; // Izquierda 0 GRADOS
	 8'b111111101:   grados=8'b01011010; // Centro       90 
	 8'b111111011:   grados=8'b10110100; // Derecha      180
	 default:        grados=8'b00000000; // Aletorio      0
	 endcase
	 end
	 

	 
//divisorfrec1	divisorfrec180	  (.clk(clk),.CLKOUT1(CLKOUT1));	 //180KHz 1-2ms
//divisorfrec	   divisorfrec200   (.clk(clk),.CLKOUT2(CLKOUT2));  //100KHz 0.6-2.4ms
divisorfrecgen  divisorfrec225  (.clk(clk),.CLKOUT3(CLKOUT3));  //94.74KHz 0.5-2.4ms

//generadorpmw     generadorpmw180    ( .CLKOUT1(CLKOUT1),.grados(grados),.PMW1(PWM1));
//generadorpmw2    generadorpmw200    ( .CLKOUT2(CLKOUT2),.grados(grados),.PMW2(PWM2));
generadorpmw3    generadorpmw225    ( .CLKOUT3(CLKOUT3),.grados(grados),.PMW3(PWM3));

endmodule