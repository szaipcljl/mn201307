--[[
History:
  2017-1-12 - [Ming Wen] created file

Copyright (c) 2016 Ambarella, Inc.

This file and its contents ("Software") are protected by intellectual
property rights including, without limitation, U.S. and/or foreign
copyrights. This Software is also the confidential and proprietary
information of Ambarella, Inc. and its licensors. You may not use, reproduce,
disclose, distribute, modify, or otherwise prepare derivative works of this
Software or any portion thereof except pursuant to a signed license agreement
or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
In the absence of such an agreement, you agree to promptly notify and return
this Software to Ambarella, Inc.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

--]]

vsrc_0 = {
	vsrc_id = 0,
	mode = "0",
	hdr_mode = "linear", -- options: "linear", "2x" or "3x"
	fps = 30,
}

vsrc_1 = {
	vsrc_id = 1,
	mode = "1080p",
	hdr_mode = "linear", -- options: "linear", "2x" or "3x"
	fps = 30,
}

chan_0 = {
	id = 0,
	vsrc = vsrc_0,
	vsrc_ctx = 0,
	img_stats_src_chan = "chan_0",
	sensor_ctrl = 1,
	max_padding_width = 0,
	idsp_fps = 0,
	lens_warp = 0,
	max_main_input_width = 0, -- 0: VIN raw width
	blend_left_or_top = 0,
	blend_right_or_bot = 0,
	main = {
		max_output = {1280, 0},
		input      = {0, 0, 1280, 720},
		output     = {0, 0, 1280, 720},
	},
	second = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
	third = {
		max_output = {1280, 0},
		input      = {0, 0, 1280, 720},
		output     = {0, 0, 320, 576},
	},
	fourth = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
	fifth = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
}

chan_1 = {
	id = 1,
	vsrc = vsrc_1,
	vsrc_ctx = 0,
	img_stats_src_chan = "chan_1",
	sensor_ctrl = 1,
	max_padding_width = 0,
	idsp_fps = 0,
	lens_warp = 0,
	max_main_input_width = 0, -- 0: VIN raw width
	blend_left_or_top = 0,
	blend_right_or_bot = 0,
	main = {
		max_output = {1920, 0},
		input      = {0, 0, 1920, 1080},
		output     = {0, 0, 1920, 1080},
	},
	second = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
	third = {
		max_output = {1920, 0},
		input      = {0, 0, 1920, 1080},
		output     = {320, 0, 400, 576},
	},
	fourth = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
	fifth = {
		max_output = {0, 0},
		input      = {0, 0, 0, 0},
		output     = {0, 0, 0, 0},
	},
}


_multi_channel_config_ = {
	version = 1,
	channels = {
		chan_0,
		chan_1,
	},
	canvas = {
		{
			type = "encode",
			size = {1280, 720},
			--[[you can change the channel orders by setting the item orer.
			Like "source = {"chan_0.main", "chan_1.main",}" or "source = {"chan_1.main", "chan_0.main",}"
			for different channel order
			--]]
			source = {"chan_0.main", },
			extra_dram_buf = 0,
		},
		{
			type = "encode",
			size = {1920, 1080},
			source = {"chan_1.main", },
			extra_dram_buf = 0,
		},
		{
			type = "prev",
			size = {720, 576},
			source = {"chan_0.third", "chan_1.third",},
			vout_id = 1,
			extra_dram_buf = 0,
		},
	},
}

return _multi_channel_config_
