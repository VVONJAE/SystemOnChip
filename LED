module LED
(
    input wire [31:0] LED_IN,
    output wire [7:0] LED_OUT
);

reg [7:0] LED_DATA = 8'b0;

always@(LED_IN)
begin
    LED_DATA[4] <= 1'b0;
    LED_DATA[5] <= 1'b0;
    LED_DATA[6] <= 1'b0;
    LED_DATA[7] <= 1'b0;
    if (LED_IN[0] == 0) begin
        LED_DATA[0] <= 1'b0;
    end
    if(LED_IN[0] == 1) begin
       LED_DATA[0] <= 1'b1;
    end
    if (LED_IN[1] == 0) begin
       LED_DATA[1] <= 1'b0;
    end
    if(LED_IN[1] == 1) begin
        LED_DATA[1] <= 1'b1;
    end
    if (LED_IN[2] == 0) begin
        LED_DATA[2] <= 1'b0;
    end
    if(LED_IN[2] == 1) begin
        LED_DATA[2] <= 1'b1;
    end
    if (LED_IN[3] == 0) begin
        LED_DATA[3] <= 1'b0;
    end    
    if(LED_IN[3] == 1) begin
        LED_DATA[3] <= 1'b1;
    end
end

assign LED_OUT = LED_DATA;

endmodule

