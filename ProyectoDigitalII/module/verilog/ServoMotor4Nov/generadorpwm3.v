module	generadorpmw3(
			input CLKOUT3,
			input [7:0]grados,
			output PMW3
			
			);


reg [11:0] counter = 0;

always@(posedge CLKOUT3) 
 begin
      if(counter<1800) counter<= counter +1;
      else counter <= 0;
end
assign PMW3 = (counter<(grados+8'b00101101)) ? 1:0;

endmodule