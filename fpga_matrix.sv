/////////////////////////////////////////////
// HMC E155 19 Monica Yao 
// myao@g.hmc.edu
/////////////////////////////////////////////

module myao_final(input logic clk, reset, 
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						//output logic [3:0] row,
						output logic a, b, c, d,
						output logic lat, oe, led_clk,
						output logic colRate);
                    
	logic [7:0] led_data;
	logic [3:0] row;
	logic sclk;
   
	slow_clk sclock(clk, reset, sclk);
	test_display test(row, led_data);
	
	led_matrix matrix(sclk, reset, led_data, r1, g1, b1, r2, g2, b2, row, a, b, c,d, lat, oe, led_clk, colRate);
	
	 
endmodule


module led_matrix(input logic clk, reset, 
						input logic [7:0] led_data, // led_data changes every cycle (it is readings from the SPI)
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						output logic [3:0] row,
						output logic a, b, c, d,
						output logic lat, oe, led_clk,
						output logic colRate);
						
	logic [4:0] column; // this is a counter that counts from 0 to 31 (which selects which column we're on)
	logic [2:0] rgb1, rgb2; 
	//logic [3:0] row;
	
	assign rgb1 = led_data[5:3];
	assign rgb2 = led_data[2:0];
	
	// assign the colors at each clk cycle (shift register)
	
	always_ff @(posedge clk, posedge reset)
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
				//{a, b, c, d} <= row;
				row <= (column == 5'b11111)? row + 1 : row; 
			end 
	
	// assign the rest of the outputs 
	always_comb
		if (column == 5'b01111)
			begin 
				lat = 0; // end of a row of data
				oe = 0; // transition from one row to next
			end	
		else if (column == 5'b10000)
			begin 
				lat = 1; // end of a row of data
				oe = 0; 
			end	
		else if (column == 5'b0)
			begin 
				lat = 0;
				oe = 1;  // clear the row of data
			end
		else 
			begin 
				lat = 0; // end of a row of data
				oe = 0; // transition from one row to next
			end	
		
	//assign row = (column == 4'b1111)? row + 1 : row; 
	assign led_clk = clk; 
	assign colRate = (column == 4'b1111);
	assign a = row[3];
	assign b = row[2];
	assign c = row[1];
	assign d = row[0];
	
endmodule 

module slow_clk(input logic clk, reset,  
						output logic slowclk); 
		
		// use a counter to change the frequency of the slowclk 
		// with n as 17 and k as 1, f_slowclk = 40Mhz/2^17 ~305Hz 
		logic [16:0] counter; 
		
		always_ff @(posedge clk)
			if (reset) counter <= 0;
			else		counter <= counter + 1;
				  
			
		assign slowclk = counter[16];
endmodule 

module test_display(input logic [3:0] row, 
							output logic [7:0] led_data); 
							
	/*always_comb
		if (row == 4'b0000) 
			//led_data <= 8'b00_111_011;
		else if (row == 4'b0100) 
			led_data <= 8'b00_111_010;
		else 
			led_data <= 8'b00_100_000;*/
	assign led_data =  8'b11_111_111;
							
endmodule 

module testbench();
    logic clk, reset;//, load, done, sck, sdi, sdo;
    logic [7:0] led_data;
	 logic r1, g1, b1, r2, g2, b2, a, b, c, d, lat, oe;
	 logic led_clk, colRate;
	 logic [3:0] row;
	 logic [8:0] i;
    
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


/*module spi(input  logic sck,  
               input  logic sdi,
               output logic sdo,
               input  logic done,
               output logic [127:0] key, plaintext,
               input  logic [127:0] cyphertext);

    logic         sdodelayed, wasdone;
    logic [127:0] cyphertextcaptured;
               
    // assert load
    // apply 256 sclks to shift in key and plaintext, starting with plaintext[0]
    // then deassert load, wait until done
    // then apply 128 sclks to shift out cyphertext, starting with cyphertext[0]
    always_ff @(posedge sck)
        if (!wasdone)  {cyphertextcaptured, plaintext, key} = {cyphertext, plaintext[126:0], key, sdi};
        else           {cyphertextcaptured, plaintext, key} = {cyphertextcaptured[126:0], plaintext, key, sdi}; 
    
    // sdo should change on the negative edge of sck
    always_ff @(negedge sck) begin
        wasdone = done;
        sdodelayed = cyphertextcaptured[126];
    end
    
    // when done is first asserted, shift out msb before clock edge
    assign sdo = (done & !wasdone) ? cyphertext[127] : sdodelayed;
endmodule*/
