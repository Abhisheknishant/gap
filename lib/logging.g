Tracing := AtomicRecord ( rec ( Trace := false,
                   StartTime := 0,
                   FileNameNo := ShareObj([2]),
                   Files := AtomicList ([])));

MicroSeconds := function()
  local t;
  t := CurrentTime();
  return t.tv_sec * 1000000 + t.tv_usec;
end;

MSTime := function()
  return MicroSeconds() - Tracing.StartTime;
end;

PostProcessLogging := function ()
  local files, lastInd, data, i, fp,
        minTime, toWrite, toRead, minInd, outFp;
  
  i := 1; files := []; data := [];
  outFp := IO_File("tmp/gap.gaplog","w");
  
  while true do
    fp := IO_File(Concatenation("tmp/w",String(i)),"r");
    if IsIdenticalObj (fp, fail) then
      lastInd := i;
      break;
    else
      files[i] := fp;
      data[i] := SplitString(IO_ReadLine(fp)," ");
      i := i+1;
    fi;
  od;
  
  repeat
    minTime := -1;
    for i in [1..lastInd] do
      if IsBound(data[i]) then
        if minTime = -1 or Int(data[i][1])<minTime then
          minTime := Int(data[i][1]);
          minInd := i;
        fi;
      fi;
    od;
    if minTime <> -1 then
      toWrite := Concatenation(data[minInd][1]," ",data[minInd][2], " ", data[minInd][3]);
      IO_Write(outFp, toWrite);
      toRead := IO_ReadLine(files[minInd]);
      if IsEmptyString(toRead) then
        Unbind(data[minInd]);
        IO_Close(files[minInd]);
        Exec(Concatenation("rm -rf tmp/w",String(minInd)));
      else
        data[minInd] := SplitString(toRead," ");
      fi;
    fi;
  until minTime = -1;
  
  IO_Close(outFp);
      
  
end;

StopLogging := function()
  Tracing.trace := false;
  IO_Write (Tracing.Files[1], MSTime(), " 0 WORKER_TASK_FINISHED\n");
  IO_Write (Tracing.Files[1], MSTime(), " 0 WORKER_FINISHED\n");
  IO_Close (Tracing.Files[1]);
  PostProcessLogging();
end;

StartLogging := function()
  local loadIO;
  Tracing.Trace := true;
  loadIO := LoadPackage("io");
  if IsIdenticalObj(loadIO,fail) then
    Print ("IO package failed to load. Logging disabled\n");
    StopLogging();
  else
    Tracing.StartTime := MicroSeconds();
    Tracing.Files[1] := IO_File("tmp/w1","w");
    IO_Write (Tracing.Files[1], MSTime(), " 0 WORKER_CREATED\n");
    IO_Write (Tracing.Files[1], MSTime(), " 0 WORKER_TASK_STARTED\n");
  fi;
end;

Tracing.InitWorkerLog := function()
  local fname, threadId;
  if (Tracing.Trace) then
    atomic Tracing.FileNameNo do
      fname := Concatenation ("tmp/w", String(Tracing.FileNameNo[1]));
      Tracing.FileNameNo[1] := Tracing.FileNameNo[1]+1;
    od;
    threadId := ThreadID(CurrentThread());
    Tracing.Files[threadId+1] := IO_File(fname,"w");
    IO_Write(Tracing.Files[threadId+1], 
            MSTime(), " ", threadId, " WORKER_CREATED\n");
  fi;
end;

Tracing.TraceWorkerSuspended := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_SUSPENDED\n");
  fi;
end;

Tracing.Close := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId, " WORKER_FINISHED\n");
    IO_Close(Tracing.Files[threadId+1]);
  fi;
end;

Tracing.TraceWorkerGotTask := function ()        
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_GOT_TASK\n");
  fi;
end;

Tracing.TraceWorkerIdle := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_IDLE\n");
  fi;
end;

Tracing.TraceTaskStarted := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_TASK_STARTED\n");
  fi;
end;

Tracing.TraceTaskFinished := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_TASK_FINISHED\n");
  fi;
end;

Tracing.TraceWorkerBlocked := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_BLOCKED\n");
  fi;
end;

Tracing.TraceWorkerResumed := function ()
  if (Tracing.Trace) then
    IO_Write(Tracing.Files[threadId+1], MSTime(), " ", threadId," WORKER_RESUMED\n");
  fi;
end;

Tracing.TraceTaskCreated := function ()
  local tracingTime;
  if (Tracing.Trace) then
    tracingTime := MSTime();
    IO_Write(Tracing.Files[threadId+1], tracingTime, " ", threadId, " WORKER_BLOCKED\n");
    IO_Write(Tracing.Files[threadId+1], tracingTime, " ", threadId, " WORKER_TASK_CREATED\n");
    IO_Write(Tracing.Files[threadId+1], tracingTime, " ", threadId, " WORKER_RESUMED\n");
  fi;
end;