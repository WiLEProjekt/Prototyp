B = [11.71,10.90,11.87,10.95,10.75,11.57,11.21,11.39,11.88,9.59,12.68,11.50,10.73,11.64,11.61]';
C = [13.70,12.13,11.41,12.75,11.91,10.66,12.91,11.02,11.37,11.09,10.51,12.28,11.79,12.03,12.24]';

group = [ 2 * ones(size(B)); 3 * ones(size(C))];

figure

boxplot([ B; C],group, 'Labels', {'TCP_PICTURE','QUIC_PICTURE'})
title('Throughput (Downlink)');
ylabel('MBit/Sec');
xlabel('ORIGIN','3');
set(gca,'XTickLabel',{,'B','C'})