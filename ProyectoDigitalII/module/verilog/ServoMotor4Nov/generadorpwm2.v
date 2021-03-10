module	generadorpmw2(
			input CLKOUT2,
			input [7:0]grados,
			output PMW2
			
			);


reg [11:0] counter = 0;

always@(posedge CLKOUT2) 
 begin
      if(counter<2000) counter<= counter +1;
      else counter <= 0;
end
assign PMW2 = (counter<(grados+8'b00111100)) ? 1:0;

endmodule