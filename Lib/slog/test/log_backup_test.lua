#!/usr/bin/lua

local slog = require("slog")


local log_from = "log_data/2.log.1"
local log_to   = "log_back/2.log"
local writer = slog.create_writer(log_from)
writer:backup(log_to)