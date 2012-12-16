-- function taken from SO answer: http://stackoverflow.com/questions/1340230/check-if-directory-exists-in-lua
function directory_exists(path)
  local f  = io.popen("cd " .. path)
  local ff = f:read("*all")

  if (ff:find("ItemNotFoundException")) then
    return false
  else
    return true
  end  
end


-- reloads specified module
function reload(what)
  package.loaded[what] = nil
  require(what)
end


if directory_exists('%windir%\\SysWOW64') then
  path_to_cmd = '%windir%\\Sysnative\\cmd.exe'
else
  path_to_cmd = '%windir%\\System32\\cmd.exe'
end

-- spawns a process
function spawn(what)
  return os.execute(path_to_cmd .. ' /c start ' .. what)
end

