
module	generadorpmw(
			input CLKOUT1,
			input [7:0]grados,
			output PMW1
			
			);


reg [11:0] counter = 0;

always@(posedge CLKOUT1) 
 begin
      if(counter<3600) counter<= counter +1;
      else counter <= 0;
end
assign PMW1 = (counter<(grados+8'b10110100)) ? 1:0;

endmodule