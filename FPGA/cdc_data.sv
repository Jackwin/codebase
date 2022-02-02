/*
Copyright 2022 Chunjie Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FRAM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

`timescale 1ns/1ps

module cdc_data #(
    parameter DATA_WIDTH = 2,
    parameter DELAY_CYCLE = 1
) (
    input logic                     dst_clk,
    input logic [DATA_WIDTH-1:0]    i_data,

    output logic [DATA_WIDTH-1:0]   o_data
);


if (DELAY_CYCLE == 0) begin
    assign o_data = i_data;
end else if (DELAY_CYCLE == 1) begin
    logic [DATA_WIDTH-1:0]  data_r;
    always_ff @(posedge dst_clk) begin
        data_r <= i_data;
    end
    assign o_data = data_r;
end else begin
    logic [DATA_WIDTH-1:0] data_r[0:DELAY_CYCLE-1];

    always_ff @(posedge dst_clk) begin
        data_r[0:DELAY_CYCLE-1] <= {data_r[1:DELAY_CYCLE-1],
                                    i_data};
    end
    assign o_data = data_r[0];
end

endmodule