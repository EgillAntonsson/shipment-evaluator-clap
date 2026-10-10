local plugin_name = "CLAP: The Shipment Evaluator (Egill Antonsson)"
local master = reaper.GetMasterTrack(0)

local confirmed = reaper.ShowMessageBox(
    "This will add a fresh Shipment Evaluator to the Master track and remove any " ..
    "existing Shipment Evaluator instances from that track. Continue?",
    "Replace Shipment Evaluator on Master",
    1
)

if confirmed ~= 1 then
    return
end

local previous_count = reaper.TrackFX_GetCount(master)
reaper.Undo_BeginBlock2(0)
reaper.PreventUIRefresh(1)
local new_index = reaper.TrackFX_AddByName(master, plugin_name, false, -1)

if new_index < 0 then
    reaper.PreventUIRefresh(-1)
    reaper.Undo_EndBlock2(0, "Add Shipment Evaluator to Master track", -1)
    reaper.ShowMessageBox(
        "REAPER could not add " .. plugin_name ..
        ". Confirm the CLAP scan completed and the plugin appears in the FX browser.",
        "Shipment Evaluator was not added",
        0
    )
    return
end

local removed_count = 0
for fx_index = previous_count - 1, 0, -1 do
    local _, fx_name = reaper.TrackFX_GetFXName(master, fx_index, "")
    if string.find(string.lower(fx_name), "shipment evaluator", 1, true) then
        reaper.TrackFX_Delete(master, fx_index)
        removed_count = removed_count + 1
    end
end

new_index = new_index - removed_count
reaper.TrackFX_Show(master, new_index, 3)
reaper.PreventUIRefresh(-1)
reaper.Undo_EndBlock2(0, "Replace Shipment Evaluator on Master track", -1)
reaper.UpdateArrange()
