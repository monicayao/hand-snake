/////////////////////////////////////////////
// HMC E155 19 Monica Yao 
// myao@g.hmc.edu
/////////////////////////////////////////////

module myao_final(input logic clk, reset, 
						input logic sck, sdi, 
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						output logic a, b, c, d,
						output logic lat, oe, led_clk);
                    
	logic [7:0] led_data;
	logic [3:0] row;
	logic sclk;
   
	//slow_clk sclock(clk, reset, sclk);
	//test_display test(row, led_data);
	
	led_matrix matrix(sclk, reset, led_data, r1, g1, b1, r2, g2, b2, row, a, b, c,d, lat, oe, led_clk);
	
	spi_slave_read spi(sck, reset, sdi, sclk, led_data);
	
endmodule


module led_matrix(input logic clk, reset, 
						input logic [7:0] led_data, // led_data changes every cycle (it is readings from the SPI)
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						output logic [3:0] row,
						output logic a, b, c, d,
						output logic lat, oe, led_clk);
						
	logic [5:0] column; // this is a counter that counts from 0 to 31 (which selects which column we're on)
	logic [2:0] rgb1, rgb2; 
	
	assign rgb1 = led_data[5:3];
	assign rgb2 = led_data[2:0];
	
	// assign the colors at each clk cycle (shift register)
	
	/*always_ff @(posedge clk, posedge reset)
		if (reset)
			begin 
				{r1, g1, b1} <= 0; 
				{r2, g2, b2} <= 0; 
				column <= 0;
				row <= 0;
			end
		else 
			begin 
				{r1, g1, b1} <= rgb1; 
				{r2, g2, b2} <= rgb2; 
				column <= column + 1;
				row <= (column == 5'b11111)? row + 1 : row; 
			end */
			
	always_ff @(negedge clk, posedge reset)
		if (reset)
			begin 
				{r1, g1, b1} <= 0; 
				{r2, g2, b2} <= 0; 
				column <= 0;
				row <= 0;
			end
		else if (column == 6'd33)
			begin 
				{r1, g1, b1} <= rgb1; 
				{r2, g2, b2} <= rgb2; 
				column <= 6'd0;
				row <= row + 4'b1; 
			end
		else
			begin 
				{r1, g1, b1} <= rgb1; 
				{r2, g2, b2} <= rgb2; 
				column <= column+ 6'b1;
				row <= row; 
			end
	
	// assign the rest of the outputs 
	/*always_comb
		if (column == 5'b11111)
			begin 
				lat = 0; // end of a row of data
				oe = 0; // transition from one row to next
			end	
		else if (column == 5'b00000)
			begin 
				lat = 1; // end of a row of data
				oe = 0; 
			end	
		else if (column == 5'b1)
			begin 
				lat = 0;
				oe = 1;  // clear the row of data
			end
		else 
			begin 
				lat = 0; // end of a row of data
				oe = 0; // transition from one row to next
			end	*/
			
	/*always_ff @(negedge clk)
		if (column == 5'b11111)
			begin 
				lat = 1; // end of a row of data
				oe = 1;
			end
		else
			begin 
				lat = 0;
				oe = 0;
			end*/
			
	always_ff @(negedge clk)
		if (column == 6'd32)
			begin 
				lat = 1; // end of a row of data
				oe = 0;
			end
		else if (column == 6'd33)
			begin 
				lat = 0; // end of a row of data
				oe = 1;
			end
		else
			begin 
				lat = 0;
				oe = 0;
			end

	assign led_clk = clk; 
	assign a = row[0];
	assign b = row[1];
	assign c = row[2];
	assign d = row[3];
	
endmodule 


module slow_clk(input logic clk, reset,  
						output logic slowclk); 
		
		// use a counter to change the frequency of the slowclk 
		// with n as 17 and k as 1, f_slowclk = 40Mhz/2^17 ~305Hz 
		logic [7:0] counter; 
		
		always_ff @(posedge clk)
			if (reset) counter <= 0;
			else		counter <= counter + 1;
				  
			
		assign slowclk = counter[7];
endmodule 

module test_display(input logic [3:0] row, 
							output logic [7:0] led_data); 
							
	/*always_comb
		if (row == 4'b1000) 
			led_data <= 8'b00_111_011;
		//else if (row == 4'b1111) 
			//led_data <= 8'b00_111_010;
		else if (row == 4'b0000)
			led_data <= 8'b00_111_101;
		else if (row == 4'b0100)
			led_data <= 8'b00_111_110;
		else if (row == 4'b1110)
			led_data <= 8'b00_111_111;
		else if (row == 4'b1110)
			led_data <= 8'b00_111_111;
		else 
			led_data <= 8'b00_000_000;
	//assign led_data =  8'b11_111_111;*/
	always_comb
		if (row == 4'b0000) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd1) 
			led_data <= 8'b00_111_100;
		else if (row == 4'd2) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd3) 
			led_data <= 8'b00_111_001;
		else if (row == 4'd4) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd5) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd6) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd7) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd8) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd9) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd10) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd11) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd12) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd13) 
			led_data <= 8'b00_111_011;
		else if (row == 4'd14) 
			led_data <= 8'b00_111_010;
		else if (row == 4'd15) 
			led_data <= 8'b00_111_011;
		else
			led_data <= 8'b00_000_010;
			
							
endmodule 

module testbench();
    logic clk, reset;//, load, done, sck, sdi, sdo;
    logic [7:0] led_data;
	 logic r1, g1, b1, r2, g2, b2, a, b, c, d, lat, oe;
	 logic led_clk, colRate;
	 logic [3:0] row;
	 logic [8:0] i;
    
	 //slow_clk sclock(clk, reset, sclk);
	 
    // device under test
    led_matrix dut(clk, reset, led_data, r1, g1, b1, r2, g2, b2, row, a, b, c,d, lat, oe, led_clk, colRate);
	
    
    // test case
    initial begin   
		led_data <= 8'b00_100_010;
		reset <= 0;
    end
    
    // generate clock and load signals
    initial 
        forever begin
            clk = 1'b0; #5;
            clk = 1'b1; #5;
        end
        
    initial begin
      i = 0;
    end 
    
	  always @(posedge clk) begin
      /*if (i == 256) load = 1'b0;
      if (i<256) begin
        #1; sdi = comb[255-i];
        #1; sck = 1; #5; sck = 0;
        i = i + 1;
      end else if (done && i < 384) begin
        #1; sck = 1; 
        #1; cyphertext[383-i] = sdo;
        #4; sck = 0;
        i = i + 1;
      end else if (i == 384) begin
            if (cyphertext == expected)
                $display("Testbench ran successfully");
            else $display("Error: cyphertext = %h, expected %h",
                cyphertext, expected);
            $stop();
      
      end*/
		if (i<10) reset <= 1;
		else reset <= 0;
		
		if (i<1024) i = i + 1;
    end
    
endmodule


module spi_slave_read(input  logic sck, reset, 
               input  logic sdi,
               output  logic done,
               output logic [7:0] led_data);

    logic [2:0] byte_count;
    
	 always_ff@(negedge sck, posedge reset)
		if (reset) byte_count = 0;
		else byte_count = byte_count + 3'b1;
	 
    
	 // shift register for the input
    always_ff @(posedge sck) 
        led_data <= (byte_count == 0)? {7'bxxxxxxx, sdi} : {led_data[6:0], sdi};
    
    // it is done with the byte count hits 7 ( this is used as clk for leds )
    assign done = (byte_count == 3'd7);
endmodule
