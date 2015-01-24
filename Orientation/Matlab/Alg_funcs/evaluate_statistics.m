function stats_struct=evaluate_statistics(data,reference_str)

stats_struct.mean=mean(data);
stats_struct.var=var(data);

% 
% if (strcmp(axe,'x')
% stats_struct.x.mean=mean(data);
% stats_struct.x.var=var(data);
% elseif (strcmp(axe,'y')
% stats_struct.y.mean=mean(data);
% stats_struct.y.var=var(data);
% elseif (strcmp(axe,'z')
% stats_struct.z.mean=mean(data);
% stats_struct.z.var=var(data);
% else
% 
% error('Parameter %s invalid',axe);
% 
% end
if ( strcmp(reference_str,'body') || strcmp(reference_str,'earth'))

stats_struct.reference=reference_str;

else

error('Parameter %s invalid',reference_str);

end



end