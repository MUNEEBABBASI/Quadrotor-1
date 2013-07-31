function getz_fin(s,init)
    fclose(s);
    clf();
    No_of_bytes=3;
    if(init==1)
        s.BaudRate=115200;
        fopen(s);
        %Clearing the buffer
        while(fread(s, 1)~='A')
            disp('Waiting');
        end
        fwrite(s,'a');
        disp('Sent a');
        if(s.BytesAvailable>0)
            fread(s,s.BytesAvailable);
        end
        disp('Channel initialized');
        pause(1);
    end
    T=5;
    t=4; %Keeps track of the number of elements of zrec
    zrec=double(zeros(1,t));
    zfilt=double(zeros(1,t));
    zrecnew=double(zeros(1,t));
    B3=[0.4097,-0.1129;0.4968,-0.1290;0.2613,-0.0484;-0.2968,0.1290]';
    fA=[-1.6169    0.6794];
    fB=[0.0164    0.0296    0.0164];
    tstart=tic;
    while(1)
%         telapsed=toc(tstart);
%         textra=tic;
%         while(telapsed<.1)
%             telapsed=toc(tstart);            
%         end
%         fprintf('Extra time -> %1.4f \n',toc(textra));
%         tstart=tic;
        fwrite(s,'f');
        while(s.BytesAvailable~=No_of_bytes)
        end
        a=fread(s,No_of_bytes,'char');  
        z=double(leftshift(uint16(a(1)),8)+uint16(a(2)));
        zrec=[zrec,z];
        t=t+1;
        if(t==T)
            zo=fB(1)*zrec(t);
        elseif(t==T+1)
            zo=fB(1)*zrec(t)+fB(2)*zrec(t-1)-fA(1)*zfilt(t-1);
        else 
            zo=fB(1)*zrec(t)+fB(2)*zrec(t-1)+fB(3)*zrec(t-2)-fA(1)*zfilt(t-1)-fA(2)*zfilt(t-2);
        end        
        zfilt=[zfilt,zo];
        z=zo;
        
        %%Prediction cell:
        base=double(zfilt(t-4));
        fifo=double(zfilt(t-3:t))-base;
        p3=B3*fifo';
        znew=double([4 16]*p3+base);
        zrecnew=[zrecnew,znew];
        
%         fprintf('%1.4f',telapsed);
        fprintf(' %3d Height =%4.2f cm | Filtered height =%4.2f cm | Fitted height =%4.2f cm \n',[t,z,zo,znew]);
        
        figure(1);
        hold on;
        plot(zrec','rx-');
        plot(zfilt','o-');
        plot(zrecnew','k-s');
        axis([t-20 t+20 0 1040]);
        if(t==T+1)
           legend('z','z-filt','z-fit');
            axis manual;
            grid on;
            title('Height measured');            
        end
        drawnow;        
    end    
end